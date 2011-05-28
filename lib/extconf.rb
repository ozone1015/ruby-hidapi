require 'mkmf'  
   
unless find_header('hidapi/hidapi.h', *['/usr/local/include'])
  abort "hidapi header is missing.  please install hidapi"
end

unless find_library('hidapi', 'hid_enumerate', *['/usr/local/lib'])
  abort "hidapi library is missing.  please install hidapi"
end
   
dir_config('ruby_hidapi')  
create_makefile('ruby_hidapi')