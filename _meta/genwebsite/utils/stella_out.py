def error(msg):
  print('\033[1;41m' + msg + ' Exit.\033[1;m')
  exit(1)

def verbose(msg):
  print('\033[1;30m' + msg + '\033[1;m')
