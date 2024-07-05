import asyncio
import socket
import uuid
from datetime import datetime
import json
from time import sleep

users = {}


def datetime_format(datetime_value: datetime):
    return f"{datetime_value:%d.%m.%Y.%H.%S}"


def generate_hash(data):
    return abs(hash(data['machine']+data['domain']+data['ip']))


def serialize_to_json(data: bytes):
    connected_at = datetime.now()
    now_str = datetime_format(connected_at)
    data_json = json.loads(data)
    data_json['ip'] = client_socket.getpeername()[0]
    data_json['domain'] = socket.gethostbyaddr(data_json['ip'])[0]
    data_json['connected_at'] = connected_at
    data_json['connected_at_str'] = now_str
    data_json['status'] = 'online'
    return data_json


async def disconnect_inactive(user_json: dict[str, str | datetime]):
    while True:
        if (datetime.now() - user_json['connected_at']).seconds >= 10:
            print("DISCOUNTING: ", user_json['user'])
            user_json['status'] = "offline"
            user_json['last_activity'] = datetime_format(datetime.now())
            client_socket.close()
            print(users)
            break
        await asyncio.sleep(3)


def set_first_connection(user_id: int, user_json):
    if user_id not in users:
        user_json['first_connection_at'] = datetime.now()
        user_json['first_connection_at_str'] = datetime_format(datetime.now())


async def handle(data):
    data_json = serialize_to_json(data)
    user_id = generate_hash(data_json)
    set_first_connection(user_id, data_json)
    users[user_id] = data_json
    await disconnect_inactive(data_json)


if __name__ == "__main__":
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(("localhost", 8081))
    sock.listen(10)

    while True:
        client_socket, client_address = sock.accept()
        data = client_socket.recv(2048)
        print("GOT", data)
        if data == b"":
            client_socket.close()
            print("DISCONNECTED")
        else:
            asyncio.run(handle(data))
