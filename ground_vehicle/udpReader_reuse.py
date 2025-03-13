import socket
import struct
import math


UDP_IP = "255.255.255.255"
UDP_PORT = 51001


# Format for the data
# 0-3 Frame Number nnnn
# 4 ItemsInBlock 2
# 5 ItemHeader:ItemID 0
# 6-7 ItemHeader:ItemDataSize 72
# 8-31 TrackerObject:ItemName 'O''b''j''e''c''t''1'00000000000000000
# 32-39 TrackerObject:TransX
# 40-47 TrackerObject:TransY
# 48-55 TrackerObject:TransZ
# 56-63 TrackerObject:RotX
# 64-71 TrackerObject:RotY
# 72-79 TrackerObject:RotZ

# Frame number uint32_T
# Items in block uint8_T
# Item header ID uint8_T
# Item header data size uint16_T
# Item name uint8_T
# Trans x double
# Trans y double
# Trans z double
# Rot x double
# Rot y double
# Rot z double


def parseInfo(data, name):
    """
    Given the data from the Vicon UDP object stream and an object's name in vicon,
    this will return the x, y, z coordinates in mm and the euler angles of the object in radians
    in the form (x, y, z, x_angle, y_angle, z_angle)

    The function will raise an Exception if the object is not in the UDP stream or the data is invalid
    """
    SHIFT = 75
    items = data[4]
    name = name.encode("utf8") + b"\x00" * (24 - len(name))
    for i in range(items):
        item_shift = SHIFT * i
        if data[8 + item_shift : 32 + item_shift] != name:
            continue
        (trans_x,) = struct.unpack("d", data[32 + item_shift : 40 + item_shift])
        (trans_y,) = struct.unpack("d", data[40 + item_shift : 48 + item_shift])
        (trans_z,) = struct.unpack("d", data[48 + item_shift : 56 + item_shift])
        (rot_x,) = struct.unpack("d", data[56 + item_shift : 64 + item_shift])
        (rot_y,) = struct.unpack("d", data[64 + item_shift : 72 + item_shift])
        (rot_z,) = struct.unpack("d", data[72 + item_shift : 80 + item_shift])

        if math.isnan(trans_x) or math.isnan(trans_y) or math.isnan(trans_z):
            break
        if trans_x == trans_y == trans_z == 0:
            break

        return trans_x, trans_y, trans_z, rot_x, rot_y, rot_z
    raise Exception()


if __name__ == "__main__":
#    OBJECT_NAME = "object"
    OBJECT_NAME = "Drone00"
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)

    sock.bind((UDP_IP, UDP_PORT))
    while True:
        data, addr = sock.recvfrom(256)
        try:
            trans_x, trans_y, trans_z, rot_x, rot_y, rot_z = parseInfo(
                data, OBJECT_NAME
            )
        except Exception as e:
            continue

        print(f"{OBJECT_NAME}:\
        {trans_x:.2f} {trans_y:.2f} {trans_z:.2f}\
        {rot_x*180/math.pi:.2f} {rot_y*180/math.pi:.2f} {rot_z*180/math.pi:.2f}"
        )
