from django import forms
from django.forms.fields import ChoiceField, MultipleChoiceField
from django.forms.widgets import RadioSelect,CheckboxSelectMultiple
from django.contrib.formtools.wizard import FormWizard
from django.shortcuts import render_to_response
from Configuration.wizard.models import Template, Label
from djangotoolbox.fields import ListField, EmbeddedModelField, DictField

# Form used to configure Ingestion data
# It also has a validate_form that is called to 
# wrapper IngestionForm values into IngestionTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper
INGESTION_MODE_CHOICES=(('Streaming','Streaming'),('File','File'))
class IngestionForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    name = forms.CharField(max_length=100)
    mode = ChoiceField(widget=RadioSelect, choices=INGESTION_MODE_CHOICES)
    path = forms.CharField(max_length=200)
    size = forms.IntegerField()
    def validate_form(self,form):
        if self.is_valid(): # All validation rules pass
            # Create IngestionMode
            dict = {}
            for choice in INGESTION_MODE_CHOICES:
                if self['mode'].value() == "Streaming":
                    dict[choice[0]] = 1
                else:
                    dict[choice[0]] = 0
            lbl1 = Label()  
            lbl1.options['IngestionMode'] = dict  
            lbl1.options["IngestionAddress"] = self['path'].value()
            lbl1.options["IngestionSize"] = self['path'].value()
            tpl = Template(template = "IngestionTemplate",attribute_values = [lbl1])
            tpl.save()
        else:
            return render_to_response('forms/ingestionForm.html')        

# Form used to configure Filters data
# It also has a validate_form that is called to 
# wrapper PreProcessingForm values into PreProcessingTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper
# {"_id":ObjectId("xxxxxxx"),
# "template":"PreprocessingTemplate","
# attribute_values":[{"header":"Extension Filter","options":[{"Enabled":1,"Values":["jpg","mp3","png"]}]},
# {"header":"Third Party Filter","options":[{"Enabled":1,"Values":["admob.com","adserve.es"]}]},
#  {"header":"Personal Info Filter","options":[{"Enabled":1,"Values":["pornhub.com","porntube.com"]}]}]}
FILTERS_CHOICES=(('1','Enabled'),('0','Disabled'))
EXTENSION_VALUES=(('jpg','jpg'),('mp3','mp3'),('png','png'))
THIRD_PARTY_VALUES=(('admob_com','admob.com'),('admob.com','admob.com'))
PERSONAL_INFO_VALUES=(('pornhub_com','pornhub.com'),('porntube.com','porntube.com'))

class PreProcessingForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    extension = ChoiceField(widget=RadioSelect, choices=FILTERS_CHOICES)
    extension_values = forms.MultipleChoiceField(choices=EXTENSION_VALUES)
    thirdParty = ChoiceField(widget=RadioSelect, choices=FILTERS_CHOICES)
    thirdParty_values = forms.MultipleChoiceField(choices=THIRD_PARTY_VALUES)
    personalInfo = ChoiceField(widget=RadioSelect, choices=FILTERS_CHOICES)
    personal_values = forms.MultipleChoiceField(choices=PERSONAL_INFO_VALUES)
    def validate_form(self,form):
        if self.is_valid(): # All validation rules pass
            dict_values = {}
            for values in self['extension_values'].value():
                dict_values[values] = 1            
            lbl1 = Label(header = "Extension Filter", options= {"Enabled": self["extension"].value(), "values" : dict_values } )
            
            dict_values = {}
            for values in self['thirdParty_values'].value():
                dict_values[values] = 1                                        
            lbl2 = Label(header = "Third Party Filter", options= {"Enabled": self["thirdParty"].value()} )
        
            dict_values = {}
            for values in self['personal_values'].value():
                dict_values[values] = 1
            lbl3 = Label(header = "Personal Info Filter", options= {"Enabled": self["personalInfo"].value(),
                                                                    "values" : dict_values } )
            
            tpl = Template(template = "PreProcessingTemplate", attribute_values = [lbl1,lbl2,lbl3])
            tpl.save()
        else:
            return render_to_response('forms/preProcessingForm.html')
        
    def values_selected(self,formField):
        dict_values = {}
        for values in self['formField']:
            dict_values[values] = 1
        return dict_values

# Form used to configure Ingestion data
# It also has a validate_form that is called to 
# wrapper IngestionForm values into IngestionTemplate Model
# that's the reason why Form is no saved, it's used like a wrapper

# WebProfiling Mongo Example
# {"_id":ObjectId("xxxxxxx"),
# "template":"WebProfilingTemplate",
# "attribute_values":[{"header":"Name", "options":"Visitors per Domain"},
# {"header":"Grouping Fields","options":[{"visitorId":0},{"protocol":1},{"urlDomain":1},.......{"status":0}]},
# {"header":"Attribute Fields","options":[{"visitorId":1},{"protocol":0},{"fullUrl":0},.......{"status":0}]},
# {"header":"Consumption","options":[{"highAvailability":0},{"dataExploitation":1}]}]

KPIS_CHOICES=(('Visitors per Domain','Visitors per Domain'),
              ('Visitors per Device','Visitors per Device'),
              ('Visitors per Protocol','Visitors per Protocol'))
GROUPING_FIELDS_CHOICES=(('1','Enabled'),('0','Disabled'))
ATTRIBUTE_FIELDS_CHOICES=(('visitorId','visitorId'),('protocol','protocol'),('fullUrl','fullUrl'),('status','status'),('urlDomain','urlDomain'))
CONSUMPTION_MODE_CHOICES=(('highAvailability','Enabled'),('dataExploitation','Disabled'))
class WebProfilingForm(forms.Form):
    error_css_class = 'error'
    required_css_class = 'required'
    mode = ChoiceField(widget=RadioSelect, choices=CONSUMPTION_MODE_CHOICES)
    grouping_fields = forms.MultipleChoiceField(   required=False, 
                                                    widget=CheckboxSelectMultiple,
                                                    choices=ATTRIBUTE_FIELDS_CHOICES)
    attributes_fields = forms.MultipleChoiceField(   required=False, 
                                                    widget=CheckboxSelectMultiple,
                                                    choices=ATTRIBUTE_FIELDS_CHOICES)
    consumptionPath = forms.CharField(max_length=200)
    def validate_form(self,form):
        if self.is_valid(): # All validation rules pass
            lbl1 = Label()
            lbl1.header = "ConsumptionMode"
            if self['mode'].value() == "Data Exploitation":
                options = "1,0"
            else:
                options = "0,1"                   
            #lbl1.options = options
            
            lbl2 = Label(header = "Grouping Fields")
            for option in self['attributes_fields'].value():
                lbl2.options[option] = 1
            
            lbl3 = Label(header = "Attributes Fields")
            for option in self['attributes_fields'].value():
                lbl3.options[option] = 1
                
            tpl = Template()
            tpl.template = "WebProfilingTemplate"
            tpl.attribute_values = [lbl1,lbl2,lbl3]
            tpl.save()
        else:
            return render_to_response('forms/error2.html')


class ConfigurationWizard(FormWizard): 
#    def get_template(self,step):
#        stp = step
#        if stp == 0:
#            return ['forms/wizard_%s.html' % step, 'wizard/ingestion.html']
#        elif stp == 1:
#            return ['forms/wizard_%s.html' % step, 'wizard/preProcessing.html']
#        elif stp == 2:
#            return ['forms/wizard_%s.html' % step, 'wizard/webProfiling.html']
        
    def done(self, request, form_list):
        for form in form_list:
            form.validate_form(form)
        return render_to_response('forms/finish.html')
        