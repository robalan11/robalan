import sys, pygame

class Game(object):
    def __init__(self):        
        self._clock = pygame.time.Clock()
    def run(self, screen):
        while True:
            # Get pressed keys
            keys = pygame.key.get_pressed()
            # Check for Alt-F4
            if ((keys[pygame.K_RALT] or keys[pygame.K_LALT]) and keys[pygame.K_F4]) or keys[pygame.K_ESCAPE]:
                sys.exit()
            
            # Check events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit()
            
            # Display things
            
            
            # Flip buffers and tick the clock
            pygame.display.flip()
            self._clock.tick(30)
