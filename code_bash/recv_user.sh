#!/bin/bash
list_user=$(ssh -p 2222 poney@79.137.34.17 "python3 -c '
import os
import sys
from django.core.management import execute_from_command_line
os.environ.setdefault(\"DJANGO_SETTINGS_MODULE\", \"chambre_froide.settings\")
sys.path.append(\"/django/site_internet/\")
import django
django.setup()
from site_web.models import User
usr=[]
for i in range(len(User.objects.all())):
  print(User.objects.all()[i].iud)
'")
echo $list_user | tr " " "\n" > "/home/poney/script/projet_si/code_raspberry/allow_users.txt"
