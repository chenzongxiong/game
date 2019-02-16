from threading import Lock
from flask import Flask, render_template, session, request, jsonify, json as flask_json
from flask_login import LoginManager, UserMixin, current_user, login_user, \
     logout_user, login_required
# from flask_session import Session
from flask_socketio import SocketIO, emit
from flask_socketio import Namespace
from flask_socketio import SocketIO, Namespace, emit, join_room, leave_room, \
    close_room, rooms, disconnect
import json
import logging


def getLogger(name="unknown"):
    date_fmt = "%Y-%m-%d %H:%M:%S"
    log_fmt = '%(asctime)s %(levelname)s %(name)s:%(lineno)d %(message)s'
    formatter = logging.Formatter(log_fmt, date_fmt)
    handler = logging.StreamHandler()
    handler.setFormatter(formatter)
    logger = logging.getLogger(name)
    logger.addHandler(handler)
    logger.setLevel(logging.DEBUG)
    return logger


LOG = getLogger(__name__)

app = Flask(__name__)
app.config['SECRET_KEY'] = 'top-secret!'
app.config['SESSION_TYPE'] = 'redis'
# login = LoginManager(app)
# Session(app)
# use redis instead of in-memory structure should be the next step
socketio = SocketIO(app, manage_session=False,
                    # message_queue="redis://localhost:6379/0",
                    # channel="zxchen_test",
                    json=flask_json)

class GameType(object):
    DEFAULT = 'default'
    EOS = 'eos'
    ETH = 'eth'


DEFAULT_TYPE = GameType.DEFAULT
DEFAULT_FEE = 1
DEFAULT_USERID = "anonymous"


@app.route('/')
def index():
    return render_template('index.html')


class DiceNamespace(Namespace):
    def __init__(self, *args, **kwargs):
        super(DiceNamespace, self).__init__(*args, **kwargs)
        self.thread = None
        self.thread_lock = Lock()
        self.user_rooms = {}

    def update_waitingboard(self):
        while True:
            LOG.debug("update waiting board in room {}".format(self.namespace))
            socketio.sleep(3)
            waiting_board = [1, 2, 3, 4]
            for room, active in self.user_rooms.items():
                if active:
                    socketio.emit('response',
                                {'event': 'update_waiting_board', 'status': 'success', 'waiting_board': waiting_board, 'type': self.namespace},
                                  room=room,
                                  namespace="/dice")

    def on_join(self, message):
        # TODO: check whether a person already be in the room. in db or in cache?
        # need session here?
        # really room needed ?
        userid = message.get('userid', None)
        typ = message.get('type', DEFAULT_TYPE).lower()
        fee = message.get('fee', DEFAULT_FEE)
        if not userid:
            room = "/waitingboard/{}".format(typ)
            join_room(room)
            emit("response", {"event": "join", "status": "success",
                               "userid": DEFAULT_USERID, "fee": fee,
                              "type": typ, "request_sid": request.sid, "room": room})
            return

        userid_type = "{}-{}".format(userid, typ)
        curr_room = '{}-{}'.format(request.sid, typ)
        # on request socket id can be in two different room, for example in type ETH/EOS
        # prev_room = json.loads(session.get((userid, typ))).get('room', None)
        cached_user = session.get(userid_type, None)
        if cached_user is not None:
            prev_room = json.loads(cached_user)["room"]
            # the same type but using a different request sid, it indicates this user should be still in that room, but he logins
            # via another browser or refresh the browser.
            # if type and requset sid are both the same, don't do anything
            # if request sid remains the same, but type change, means the user wants to enter another without leaving current room
            if typ == prev_room.split('-')[-1] and \
              prev_room.split('-')[0] != request.sid:
                leave_room(prev_room)
                self.user_rooms.pop(prev_room, None)
                join_room(curr_room)
        else:
            join_room(curr_room)


        self.user_rooms[curr_room] = True
        # use (userid, typ) -> room, our server can determine which client is connecting and deliver a message to it directly.
        session[userid_type] = json.dumps({"fee": fee, "type": typ, "room": curr_room, "request_sid": request.sid})
        emit('response', {"event": "join", "status": "success",
                          "userid": userid, "fee": fee, "type": typ, "request_sid": request.sid})
        # TODO: call smart contract on the chain
    def on_leave(self, message):
        LOG.debug("invoke leave action")
        userid = message.get('userid', None)
        if not userid:
            room = request.sid
            typ = DEFAULT_TYPE
        else:
            room = json.loads(session.get("userid"))['room']
            typ = json.loads(session.get("userid"))['type']

        leave_room(request.sid)
        leave_room("/waitingboard/{}".format(typ))
        emit('response', {'event': 'leave', 'status': 'success', 'type': typ, 'userid': userid, 'request_sid': request.sid})

    def on_toss(self, message):
        # TODO: check a person still in the room ?
        userid = message.get('userid', None)
        typ = message.get('type', None)
        if not userid or not typ:
            emit("response", {'event': 'toss', 'status': 'fail', 'userid': userid, 'type': typ})

        # TODO: call random function from chain
        dice_number = 5
        emit("response", {'event': 'toss', 'status': 'success', 'userid': userid, 'type': typ, 'dice_number': dice_number})

    def on_connect(self):
        LOG.debug("request {} connects to server successfully".format(request.sid))
        with self.thread_lock:
            if self.thread is None:
                self.thread = socketio.start_background_task(self.update_waitingboard)

        emit('response', {'status': 'success', 'event': 'connect', 'request_sid': request.sid, 'namespace': self.namespace});

        # emit('response', {'status': 'success', 'event': 'connect', 'request_sid': request.sid});

    def on_disconnect(self):
        print('Client disconnected', request.sid)

    def on_notify_order(self):
        # TODO: run in background
        userid = 0
        typ = GameType.EOS
        emit("response", {'event': 'notify_order', 'userid': userid, 'type': typ, 'request_sid': request.sid}, room=typ)

    def on_disconnect_request(self):
        emit("response", {"event": "disconnect_request", "status": "success", "request_sid": request.sid})
        disconnect()

    def on_default_request(self, message):
        LOG.debug("invoke default request...")
        emit("response", {"event": "default_request", "status": "success", "request_sid": request.sid})


class WaitingboardNamespace(Namespace):
    def __init__(self, *args, **kwargs):
        super(WaitingboardNamespace, self).__init__(*args, **kwargs)
        self.thread = None
        self.thread_lock = Lock()
        print(self.namespace)

    def update_waitingboard(self):
        while True:
            LOG.debug("update waiting board in room {}".format(self.namespace))
            socketio.sleep(3)
            waiting_board = [1, 2, 3, 4]
            socketio.emit('response',
                          {'event': 'update_waiting_board', 'status': 'success', 'waiting_board': waiting_board, 'type': self.namespace},
                          broadcast=True,
                          room=self.namespace,
                          namespace="/dice")

    def on_connect(self):
        LOG.debug("request {} connects to server successfully".format(request.sid))
        with self.thread_lock:
            if self.thread is None:
                self.thread = socketio.start_background_task(self.update_waitingboard)

        emit('response', {'status': 'success', 'event': 'connect', 'request_sid': request.sid, 'namespace': self.namespace});


# socketio.on_namespace(DiceNamespace('/dice'))
# socketio.on_namespace(WaitingboardNamespace('/waitingboard/default'))
# socketio.on_namespace(WaitingboardNamespace('/waitingboard/eos'))
# socketio.on_namespace(WaitingboardNamespace('/waitingboard/eth'))


if __name__ == '__main__':
    socketio.run(app, debug=True)
