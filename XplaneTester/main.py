import socket
import json

sim = ("0.0.0.0", 50555)

ops = {
    "Operation" : "setdata",
    "Link" : "sim/weather/rain_percent",
    "Value": "1.0"
}

so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
so.sendto(json.dumps(ops).encode(), sim)
