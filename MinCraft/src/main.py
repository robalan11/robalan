import sys, pygame
import game

def main(argv):
    pygame.init()
    size = width, height = 1024, 768
    screen = pygame.display.set_mode(size)
    black = 0, 0, 0

    g = game.Game()
    g.run(screen)

if __name__ == "__main__":
    main(sys.argv)