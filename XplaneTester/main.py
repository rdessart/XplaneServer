import socket
import json

sim = ("0.0.0.0", 50555)

ops = {
    "Dataref" : {
        "Link" : "sim/flightmodel/position/latitude",
        "Type" : "DOUBLE"
    },
    "Operation" : "getdata"
}

so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

so.bind(("0.0.0.0", 55_876))
so.sendto(json.dumps(ops).encode(), sim)
print(so.recvfrom(1024)[0].decode())
