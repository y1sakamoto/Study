import argparse
import random
import time

from pythonosc import osc_message_builder
from pythonosc import udp_client

from pythonosc import dispatcher
from pythonosc import osc_server

parser = argparse.ArgumentParser()
parser.add_argument("--ip", default="127.0.0.1",help="The ip of the OSC server")
parser.add_argument("--portSend", type=int, default=5104,help="The port the OSC server is listening on")
parser.add_argument("--PortRecieve",type=int, default=5304, help="The port to listen on")
args = parser.parse_args()




def print_volume_handler(unused_addr, args, volume):
  print("[{0}] ~ {1}".format(args[0], volume))

def print_compute_handler(unused_addr, args, volume):
  try:
    print("[{0}] ~ {1}".format(args[0], args[1](volume)))
  except ValueError: pass


def sendAction(float action):
    client = udp_client.SimpleUDPClient(args.ip, args.portSend)
    client.send_message("/action", action)



if __name__ == "__main__":
    client = udp_client.SimpleUDPClient(args.ip, args.portSend)
    for x in range(10):
        client.send_message("/filter", random.random())
        time.sleep(1)


if __name__ == "__main__":
    dispatcher = dispatcher.Dispatcher()
    dispatcher.map("/filter", print)
    dispatcher.map("/volume", print_volume_handler, "Volume")
    dispatcher.map("/logvolume", print_compute_handler, "Log volume", math.log)
    print (0)
    server = osc_server.ThreadingOSCUDPServer((args.ip, args.PortRecieve), dispatcher)
    print("Serving on {}".format(server.server_address))
    server.serve_forever()
