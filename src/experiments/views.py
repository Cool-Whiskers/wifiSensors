#### API Using Generic Class-Based Views ####
from .models import Experiment
from .serializers import ExperimentSerializer
from rest_framework import generics

class ExperimentList(generics.ListCreateAPIView):
    queryset = Experiment.objects.all()
    serializer_class = ExperimentSerializer

class ExperimentDetail(generics.RetrieveUpdateDestroyAPIView):
    queryset = Experiment.objects.all()
    serializer_class = ExperimentSerializer

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

# ##### API using function based views ######
# from rest_framework import status
# from rest_framework.decorators import api_view
# from rest_framework.response import Response
# from .models import Experiment
# from .serializers import ExperimentSerializer

# from django.shortcuts import render
# from django.http import HttpResponse, JsonResponse
# from django.views.decorators.csrf import csrf_exempt
# from rest_framework.parsers import JSONParser
# from .models import Experiment
# from .serializers import ExperimentSerializer

# # More concise and uses named status codes
# @api_view(['GET', 'POST'])
# def experiment_list(request, format=None):
#     """
#     List all experiments, or create a new experiment.
#     """
#     if request.method == 'GET':
#         experiments = Experiment.objects.all()
#         serializer = ExperimentSerializer(experiments, many=True)
#         return Response(serializer.data)
#
#     elif request.method == 'POST':
#         serializer = ExperimentSerializer(data=request.data)
#         if serializer.is_valid():
#             serializer.save()
#             return Response(serializer.data, status=status.HTTP_201_CREATED)
#         return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

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

# # No longer ties our requests or responses to a given content type
# @api_view(['GET', 'PUT', 'DELETE'])
# def experiment_detail(request, pk, format=None):
#     """
#     Retrieve, update or delete a experiment.
#     """
#     try:
#         experiment = Experiment.objects.get(pk=pk)
#     except Experiment.DoesNotExist:
#         return Response(status=status.HTTP_404_NOT_FOUND)
#
#     if request.method == 'GET':
#         serializer = ExperimentSerializer(experiment)
#         return Response(serializer.data)
#
#     elif request.method == 'PUT':
#         serializer = ExperimentSerializer(experiment, data=request.data)
#         if serializer.is_valid():
#             serializer.save()
#             return Response(serializer.data)
#         return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
#
#     elif request.method == 'DELETE':
#         experiment.delete()
#         return Response(status=status.HTTP_204_NO_CONTENT)

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
