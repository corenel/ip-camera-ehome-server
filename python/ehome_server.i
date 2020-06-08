%module ehome_server

%include <opencv.i>

%cv_instantiate_all_defaults

%{
  #include "ices/config.h"
  #include "ices/ip_camera.h"
  #include "ices/ehome_server.h"
%}

%include "ices/config.h"
%include "ices/ip_camera.h"
%include "ices/ehome_server.h"
