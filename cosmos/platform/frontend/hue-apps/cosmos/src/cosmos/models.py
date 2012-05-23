"""
Data models.

"""
import logging
from datetime import datetime

from django.contrib.auth.models import User
from django.core.urlresolvers import reverse
from django.db import models
from jobsubd.ttypes import State
from jobsub.models import Submission

from cosmos import conf


LOGGER = logging.getLogger(__name__)
PATH_MAX_LENGTH = 256
STATE_NAMES = {
    1: 'submitted',
    2: 'running',
    3: 'successful',
    4: 'failed',
    5: 'failed'   # jobsub model has two failed states that we map to failed
}


class JobRun(models.Model):
    """JobRun corresponds with an execution of a given model."""

    NAME_MAX_LENGTH = 40
    name = models.CharField(max_length=NAME_MAX_LENGTH)

    DESCRIPTION_MAX_LENGTH = 1024
    description = models.TextField(max_length=DESCRIPTION_MAX_LENGTH)
    user = models.ForeignKey(User)
    dataset_path = models.CharField(max_length=PATH_MAX_LENGTH)
    jar_path = models.CharField(max_length=PATH_MAX_LENGTH)
    start_date = models.DateTimeField(auto_now=True)
    submission = models.ForeignKey(Submission, null=True)
    last_submission_refresh = models.DateTimeField(auto_now=True)

    def __unicode__(self):
        return self.name

    def mongo_db(self):
        return 'db_%d' % self.user.id

    def mongo_collection(self):
        return 'job_%d' % self.id

    def mongo_url(self):
        return '%s/%s.%s' % (conf.MONGO_BASE.get(), self.mongo_db(),
                             self.mongo_collection())

    def hadoop_args(self, jar_name):
        input_path = self.dataset_path
        output_path = '/user/%s/tmp/job_%d/' % (self.user.username, self.id)
        return ['jar', jar_name, input_path, output_path, self.mongo_url()]

    def state(self):
        if self.submission is None:
            return 'unsubmitted'
        else:
            return STATE_NAMES[self.submission.last_seen_state]

    def in_final_state(self):
        return (self.submission is None or
                self.submission.last_seen_state > 2)

    def refresh_state(self):
        """Poll the helper Java process for a fresh state."""

        if self.in_final_state():
            return

        now = datetime.now()
        if ((now - self.last_submission_refresh).seconds <
            conf.MIN_POLLING_INTERVAL):
            return

        submission = job_run.submission
        job_data = jobsub.get_client().get_job_data(submission.submission_handle)
        submission.last_seen_state = job_data.state
        submission.save()
        self.last_submission_refresh = now
        self.save()


    def action_links(self):
        """
        Creates a vector of links to actions related to the job run.
        Each link is a dict of the form:
            {href: '/path', class: 'class', target: 'HueApp', name: 'name'}
        """
        if self.submission is None:
            return []
        links = []
        if self.submission.last_seen_state == State.SUCCESS:
            links.append({
                'name': 'Results',
                'class': 'results',
                'target': None,
                'href': reverse('show_results', args=[self.id])
            });
        links.append({
            'name': 'Detailed status',
            'class': 'status',
            'target': 'JobSub',
            'href': self.submission.watch_url()
        });
        return links
