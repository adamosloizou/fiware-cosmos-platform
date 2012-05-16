"""
Application forms and validation.

"""
import re

from django import forms
from django.core import validators

from cosmos import models


ID_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'^[a-z][a-z0-9\-_]*$', re.IGNORECASE),
    message='Enter a valid identifier (only letters, numbers and dashes' +
            'starting by a letter)')


ABSOLUTE_PATH_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'^(/\.?[^./][^/]*)+/?$'),
    message='Enter a valid absolute path')


JAR_FILE_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'\.jar$', re.IGNORECASE),
    message='Filename must have "jar" extension')


class RunJobForm(forms.Form):
    name = forms.CharField(max_length=models.JobRun.NAME_MAX_LENGTH,
                           label='Name', validators=[ID_VALIDATOR])
    description = forms.CharField(
        max_length=models.JobRun.DESCRIPTION_MAX_LENGTH,
        label='Description', required=False)
    jar_path = forms.CharField(max_length=models.PATH_MAX_LENGTH,
                               validators=[ABSOLUTE_PATH_VALIDATOR,
                                           JAR_FILE_VALIDATOR],
                               label='JAR file')
    dataset_path = forms.CharField(max_length=models.PATH_MAX_LENGTH,
                                   validators=[ABSOLUTE_PATH_VALIDATOR],
                                   label='Dataset path')
