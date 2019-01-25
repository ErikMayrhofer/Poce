# from apps.facepong import camOpener
from src.pongGame import PongGame, PONG_GAME_RESTART


ret = PONG_GAME_RESTART
while ret == PONG_GAME_RESTART:
    ret = PongGame().run()

print("Goodbye!")
