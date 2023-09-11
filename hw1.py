import time

def application_layer(message):
  print('Adding Application Layer Header')
  message = ' [Application Layer Header] ' + message
  presentation_layer(message)

def presentation_layer(message):
  print('Adding Presentation Layer Header')
  message = ' [Presentaion Layer Header] ' + message
  session_layer(message)

def session_layer(message):
  print('Adding Session Layer Header')
  message = ' [Session Layer Header] ' + message
  transport_layer(message)

def transport_layer(message):
  print('Adding Transport Layer Header')
  message = ' [Transport Layer Header] ' + message
  network_layer(message)

def network_layer(message):
  print('Adding Network Layer Header')
  message = ' [Network Layer Header] ' + message
  datalink_layer(message) 

def datalink_layer(message):
  print('Adding Data Link Layer Header')
  message = ' [Data Link Layer] ' + message
  physical_layer(message)

def physical_layer(message):
  print('Adding Physical Layer Header')
  message = ' [Physical Layer Header] ' + message
  time_end= time.time()
  time_total= time_end- time_start
  print('Full Message: ' + message)
  print('Time Elapased: ' + str(time_total) )
  return 0

message = input("Please enter a message: ")
time_start= time.time()
application_layer(message)






