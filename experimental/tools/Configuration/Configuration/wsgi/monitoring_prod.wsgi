import os
import sys

sys.path.append('/usr/local/ps/apps/bdp/Configuration')
sys.path.append('/usr/local/ps/apps/bdp/Configuration/Configuration')

os.environ['DJANGO_SETTINGS_MODULE'] = 'Configuration.settings_prod'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()