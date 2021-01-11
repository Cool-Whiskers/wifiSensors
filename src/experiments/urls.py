from django.urls import path
from experiments import views
from rest_framework.urlpatterns import format_suffix_patterns

urlpatterns = [
    # path('experiments/', views.ExperimentList.as_view()),
    # path('experiments/<int:pk>/', views.ExperimentDetail.as_view()),
    path('experiments/', views.experiment_list),
    path('experiments/<int:pk>/', views.experiment_detail),
    path('experiments/bokeh/<int:run>/', views.experiment_data_view),
]

# urlpatterns = format_suffix_patterns(urlpatterns)
