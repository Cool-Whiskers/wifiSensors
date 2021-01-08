from django.db import models

# Create your models here.
class Experiment(models.Model):
    created = models.DateTimeField(auto_now_add=True)
    sensor = models.CharField(max_length=10, blank=False)
    data_type = models.CharField(max_length=100, blank=False)
    data = models.DecimalField(max_digits=100, decimal_places=6, blank=False)

    class Meta:
        ordering = ['created']
