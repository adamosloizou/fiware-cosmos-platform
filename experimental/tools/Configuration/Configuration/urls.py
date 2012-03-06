from django.conf.urls.defaults import patterns, include, url
from Configuration.wizard.forms import IngestionForm, PreProcessingForm, WebProfilingForm, WizardForm

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'BackOffice.views.home', name='home'),
    # url(r'^BackOffice/', include('BackOffice.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),


    # Initial Page
    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),
    
    url(r'^login/$', 'django.contrib.auth.views.login'),    
    
    url(r'^wizard/', include('Configuration.wizard.urls')),
)