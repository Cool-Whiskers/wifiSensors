from rest_framework import serializers
from .models import Experiment

# class defined serializer that uses the ModelSerializer class
class ExperimentSerializer(serializers.ModelSerializer):
    class Meta:
        model = Experiment
        fields = ['id', 'sensor', 'data_type', 'data']

# # class defined serializer that uses Serializer classes
# class ExperimentSerializer(serializers.Serializer):
#     id = serializers.IntegerField(read_only=True)
#     sensor = serializers.CharField(max_length=10, required=True)
#     data_type = serializers.CharField(max_length=100, required=True)
#     data = serializers.DecimalField(max_digits=100, decimal_places=6, required=True)
#
#     def create(self, validated_data):
#         """
#         Create and return a new 'Experiment' instance, given the validated_data
#         """
#         return Experiment.objects.create(**validated_data)
#
#     def update(self, instance, validated_data):
#         """
#         Update and return an existing 'Experiment' instance, given the validated data.
#         """
#         instance.sensor = validated_data.get('sensor', instance.sensor)
#         instance.data_type = validated_data.get('data_type', instance.data_type)
#         instance.data = validated_data.get('data', instance.data)
#         instance.save()
#         return instance
