from django.shortcuts import render
from .models import Experiment

from django.views.generic import TemplateView
from django.http import HttpResponse

# Create your views here.

# class QueryView(TemplateView):
#     template_name = 'query.html'

def home_view(*args, **kwargs):
    return HttpResponse("<h1>Welcome to Home Page!</h1>")

def query_view(request, *args, **kwargs):
    identifier_input = request.GET.get('id')
    data_type_input = request.GET.get('type')
    value_input = request.GET.get('value')
    if (request.GET.get('id') and request.GET.get('type') and request.GET.get('value')):
        temp = int(value_input)
        temp = temp / 1.0e6
        value_input = str(temp)
        Experiment.objects.create(identifier = identifier_input, data_type = data_type_input, value = value_input)
    message = "Experiment received: (ID: {}), (Unit: {}), (Value: {})".format(identifier_input, data_type_input, value_input)
    template = "query.html"
    context = {
        'message' : message,
    }
    return render(request, "query.html", context)
