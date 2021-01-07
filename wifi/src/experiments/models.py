from django.db import models

# Create your models here.
class Experiment(models.Model):
    identifier = models.CharField(max_length = 10)
    data_type = models.CharField(max_length = 10)
    value = models.DecimalField(max_digits = 8, decimal_places = 4)
