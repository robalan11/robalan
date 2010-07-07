import sys, pygame
import world

class Game(object):
    def __init__(self):        
        self._clock = pygame.time.Clock()
        self._world = world.World()
        self._pos = [128, 64, 0, 0]
    
    def run(self, screen):
        while True:
            # Get pressed keys
            keys = pygame.key.get_pressed()
            # Check for Alt-F4
            if ((keys[pygame.K_RALT] or keys[pygame.K_LALT]) and keys[pygame.K_F4]) or keys[pygame.K_ESCAPE]:
                sys.exit()
            
            # Move (temp)
            if keys[pygame.K_w]:
                self._pos[3] -= 16
            if keys[pygame.K_a]:
                self._pos[2] -= 16
            if keys[pygame.K_s]:
                self._pos[3] += 16
            if keys[pygame.K_d]:
                self._pos[2] += 16
            
            if self._pos[2] >= 32:
                self._pos[0] += 1
                self._pos[2] -= 32
            if self._pos[2] < 0:
                self._pos[0] -= 1
                self._pos[2] += 32
            if self._pos[3] >= 32:
                self._pos[1] += 1
                self._pos[3] -= 32
            if self._pos[3] < 0:
                self._pos[1] -= 1
                self._pos[3] += 32
            
            # Check events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit()
            
            # Update the world
            self._world.update(self._pos)
            
            # Display things
            self._world.draw(screen, self._pos)
            
            # Flip buffers and tick the clock
            pygame.display.flip()
            self._clock.tick(30)
