# #### API Using Generic Class-Based Views ####
# from .models import Experiment
# from .serializers import ExperimentSerializer
# from rest_framework import generics
#
# class ExperimentList(generics.ListCreateAPIView):
#     queryset = Experiment.objects.all()
#     serializer_class = ExperimentSerializer
#
# class ExperimentDetail(generics.RetrieveUpdateDestroyAPIView):
#     queryset = Experiment.objects.all()
#     serializer_class = ExperimentSerializer

# #### API using mixin classes ####
# from .models import Experiment
# from .serializers import ExperimentSerializer
# from rest_framework import mixins
# from rest_framework import generics
#
# class ExperimentList(mixins.ListModelMixin, mixins.CreateModelMixin, generics.GenericAPIView):
#     queryset = Experiment.objects.all()
#     serializer_class = ExperimentSerializer
#
#     def get(self, request, *args, **kwargs):
#         return self.list(request, *args, **kwargs)
#
#     def post(self, request, *args, **kwargs):
#         return self.creaet(request, *args, **kwargs)
#
# class ExperimentDetail(mixins.RetrieveModelMixin,
#                        mixins.UpdateModelMixin,
#                        mixins.DestroyModelMixin,
#                        generics.GenericAPIView):
#     queryset = Experiment.objects.all()
#     serializer_class = ExperimentSerializer
#
#     def get(self, request, *args, **kwargs):
#         return self.retrieve(request, *args, **kwargs)
#
#     def put(self, request, *args, **kwargs):
#         return self.update(request, *args, **kwargs)
#
#     def delete(self, reuqest, *args, **kwargs):
#         return self.destroy(request, *args, **kwargs)

# ##### API using class based views ######
# from .models import Experiment
# from .serializers import ExperimentSerializer
# from django.http import Http404
# from rest_framework.views import APIView
# from rest_frameowrk.reponse import Response
# from rest_framework import status
#
# class ExperimentList(APIView):
#     """
#     List all experiments, or creaet a new experiment.
#     """
#     def get(self, request, format=None):
#         experiments = Experiment.objects.all()
#         serializer = ExperimentSerializer(experiments, many=True)
#         return Response(serializer.data)
#
#     def post(self, request, format=None):
#         serializer = ExperimentSerializer(data=request.data)
#         if serializer.is_valid():
#             serializer.save()
#             return Response(serializer.data, status=status.HTTP_201_CREATED)
#         return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
#
# class ExperimentDetail(APIView):
#     """
#     Retrieve, update, or delete an experiment instance.
#     """
#     def get_object(self, pk):
#         try:
#             return Experiment.objects.get(pk=pk)
#         except Experiment.DoesNotExist:
#             raise Http404
#
#     def get(self, request, pk, format=None):
#         experiment = self.get_object(pk)
#         serializer = ExperimentSerializer(experiment)
#         return Response(serializer.data)
#
#     def put(self, request, pk, format=None):
#         experiment = self.get_object(pk)
#         serializer = ExperimentSerializer(experiment, data=request.data)
#         if serializer.is_valid():
#             serializer.save()
#             return Response(serializer.data)
#         return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
#
#     def delete(self, request, pk, format=None):
#         experiment = self.get_object(pk)
#         experiment.delete()
#         return Response(status=status.HTTP_204_NO_CONTENT)

##### API using function based views ######
from rest_framework import status
from rest_framework.decorators import api_view
from rest_framework.response import Response
from .models import Experiment
from .serializers import ExperimentSerializer

from django.shortcuts import render
# from django.http import HttpResponse, JsonResponse
# from django.views.decorators.csrf import csrf_exempt
# from rest_framework.parsers import JSONParser
# from .models import Experiment
# from .serializers import ExperimentSerializer

# View experiment data using bokeh

from bokeh.plotting import figure
from bokeh.embed import components
from bokeh.models import HoverTool

def experiment_data_view(request, run, *args, **kwargs):
    y = []
    t = []
    v = []
    experiments = Experiment.objects.filter(run=run)
    t_start_micro = experiments[0].created.microsecond
    t_start_sec = experiments[0].created.second
    t_start_min = experiments[0].created.minute
    for i in range(0, len(experiments)):
        t_micro = experiments[i].created.microsecond
        t_sec = experiments[i].created.second
        t_min = experiments[i].created.minute
        t_total = (t_micro * 1e-6) + t_sec + (t_min * 60) - ((t_start_micro * 1e-6) + t_start_sec + (t_start_min * 60))
        t.append(t_total)
        v.append(experiments[i].battery_volt)
        y.append(experiments[i].data)
    time_start = experiments[0].created
    title_temp = 'Time-Series of Temperature for Run: ' + str(run) + ', Start Time : ' + str(time_start)
    title_bat = 'Time-Series of Battery Voltage for Run: ' + str(run) + ', Start Time : ' + str(time_start)
    plot_temp = figure(title=title_temp, x_axis_label='Time(sec)', y_axis_label='Temperature(C)')
    plot_temp.line(t, y, legend="Temperature")
    plot_temp.circle(t, y, fill_color="white", size=8)

    plot_bat = figure(title=title_bat, x_axis_label='Time(sec)', y_axis_label='Voltage (V)')
    plot_bat.line(t, v, legend="Voltage", line_color="red")
    plot_bat.circle(t, v, fill_color="white", line_color="red", size=8)

    script_t, div_t = components(plot_temp)
    script_b, div_b = components(plot_bat)


    return render(request, 'data.html', {'script_t': script_t,
                                         'div_t': div_t,
                                         'script_b': script_b,
                                         'div_b': div_b})

# More concise and uses named status codes
@api_view(['GET', 'POST'])
def experiment_list(request, format=None):
    """
    List all experiments, or create a new experiment.
    """
    if request.method == 'GET':
        experiments = Experiment.objects.all()
        for i in experiments:
            print(i.created.microsecond)
        serializer = ExperimentSerializer(experiments, many=True)
        return Response(serializer.data)

    elif request.method == 'POST':
        serializer = ExperimentSerializer(data=request.data)
        if serializer.is_valid():
            serializer.validated_data['data'] = float(serializer.validated_data.get('data')) / 1.0e6
            serializer.validated_data['battery_volt'] = float(serializer.validated_data.get('battery_volt')) / 1.0e6
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

# @csrf_exempt # Usually not a good idea
# def experiment_list(request):
#     """
#     List all experiments, or create a new experiment.
#     """
#     if request.method == 'GET':
#         experiments = Experiment.objects.all()
#         serializer = ExperimentSerializer(experiments, many=True)
#         return JsonResponse(serializer.data, safe=False)
#
#     elif request.method == 'POST':
#
#         # identifier_input = request.POST.get('sensor')
#         # data_type_input = request.POST.get('data_type')
#         # value_input = request.POST.get('data')
#         #
#         # print('method is POST')
#         #
#         # if (request.POST.get('sensor') and request.POST.get('data_type') and request.POST.get('data')):
#         #     print('GOT IT')
#         #     temp = int(value_input)
#         #     temp = temp / 1.0e6
#         #     value_input = str(temp)
#         #     Experiment.objects.create(sensor = identifier_input, data_type = data_type_input, data = value_input)
#
#         data = JSONParser().parse(request)
#         serializer = ExperimentSerializer(data=data)
#         if serializer.is_valid():
#             serializer.save()
#             return JsonResponse(serializer.data, status=201)
#         return JsonResponse(serializer.errors, status=400)

# No longer ties our requests or responses to a given content type
@api_view(['GET', 'PUT', 'DELETE'])
def experiment_detail(request, pk, format=None):
    """
    Retrieve, update or delete a experiment.
    """
    try:
        experiment = Experiment.objects.get(pk=pk)
    except Experiment.DoesNotExist:
        return Response(status=status.HTTP_404_NOT_FOUND)

    if request.method == 'GET':
        serializer = ExperimentSerializer(experiment)
        return Response(serializer.data)

    elif request.method == 'PUT':
        serializer = ExperimentSerializer(experiment, data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

    elif request.method == 'DELETE':
        experiment.delete()
        return Response(status=status.HTTP_204_NO_CONTENT)

# @csrf_exempt
# def experiment_detail(request, pk):
#     """
#     Retrieve, update or delete a experiment.
#     """
#     try:
#         experiment = Experiment.objects.get(pk=pk)
#     except Experiment.DoesNotExist:
#         return HttpResponse(status=404)
#
#     if request.method == 'GET':
#         serializer = ExperimentSerializer(experiment)
#         return JsonResponse(serializer.data)
#
#     elif request.method == 'PUT':
#         data = JSONParser().parse(request)
#         serializer = ExperimentSerializer(experiment, data=data)
#         if serializer.is_valid():
#             serializer.save()
#             return JsonResponse(serializer.data)
#         return JsonResponse(serializer.errors, status=400)
#
#     elif request.method == 'DELETE':
#         experiment.delete()
#         return HttpResponse(status=204)
