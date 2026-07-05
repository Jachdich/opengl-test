import pygame, random
from math import *
scl = 8
w = 1920
h = 1080
W = w//scl
H = h//scl
pygame.init()
screen = pygame.display.set_mode((w, h))

buf = [[(0, 0, 0) for y in range(H)] for x in range(W)]

def kToRGB(k):
    temp = k / 100.0

    if temp <= 66: 
    	r = 255
    else:
    	r = temp - 60
    	r = 329.698727446 * r**-0.1332047592
    	if r < 0: r = 0
    	if r > 255: r = 255
    
        
    if temp <= 66: 
    	g = temp
    	g = 99.4708025861 * log(g) - 161.1195681661
    	if g < 0: g = 0
    	if g > 255: g = 255
    else:
    	g = temp - 60
    	g = 288.1221695283 * g**-0.0755148492
    	if g < 0: g = 0
    	if g > 255: g = 255
    
    
    if temp >= 66: 
    	b = 255
    else:
    	if temp <= 19: 
    		b = 0
    	else:
    		b = temp - 10
    		b = 138.5177312231 * log(b) - 305.0447927307
    		if b < 0: b = 0
    		if b > 255: b = 255
    	
    

    return (int(r) & 0xff), (int(g) & 0xff), (int(b) & 0xff)


block_size = 2

add = False

def clamp(x, a, b):
    if x > b: return b
    if x < a: return a
    return x

def px(x, y, c):
    if x < W and x >= 0 and y < H and y >= 0:
        buf[x][y] = (
            clamp(buf[x][y][0] + c[0], 0, 255),
            clamp(buf[x][y][1] + c[1], 0, 255),
            clamp(buf[x][y][2] + c[2], 0, 255),
        )

def coldist(a, b):
    return (a[0]-b[0])**2+(a[1]-b[1])**2+(a[2]-b[2])**2

def draw_star(pos, temperature, radius):
    luminance = temperature**0.2 * radius**5 / 4e3
    rct = (max(6-radius, 0))**1.5 * temperature**1.5
    luminance += rct / 9e8
    halo_radius = luminance ** 0.3 * 10 + 1
    luminance = clamp(luminance, 0.5, 1000000000)
    colour = kToRGB(temperature)

    halo_strength = 0.75 + clamp(0.009 * luminance**2, 0, 0.2)

    r, g, b = colour
    r += 30
    g += 30
    b += 30

    palette = []
    asd = {}
    n = 4
    frac = 0
    while frac <= 1.0:
        dist = frac**(1.0 - halo_strength)
        r_ = r * luminance * (1.0 - dist)
        g_ = g * luminance * (1.0 - dist)
        b_ = b * luminance * (1.0 - dist)
        if r_ > 255.0: r_ = 255.0
        if b_ > 255.0: b_ = 255.0
        if g_ > 255.0: g_ = 255.0
        col = (int(r_), int(g_), int(b_))
        if not col in palette:
            palette.append(col)
            asd[dist] = col
        frac += 1/n

    x0 = clamp(floor(pos[0] - halo_radius - radius), 0, W-1)
    x1 = clamp(ceil(pos[0] + halo_radius + radius), 0, W-1)
    y0 = clamp(floor(pos[1] - halo_radius - radius), 0, H-1)
    y1 = clamp(ceil(pos[1] + halo_radius + radius), 0, H-1)

    dithers = [[[1, 0],
                [0, 0]],
               [[1, 0],
                [0, 1]],
               [[1, 0],
                [1, 1]],
               [[1, 1],
                [1, 1]]]

    for x in range(x0, clamp(pos[0] +1, 0, W-1)):
        for y in range(y0, clamp(pos[1]+1, 0, H-1)):
            cx = x - pos[0]
            cy = y - pos[1]
            raw_dist = sqrt((cx)**2 + (cy)**2) - radius
            dist = raw_dist / halo_radius

            if dist < 0.0:
                dist = 0.0
            if dist > 1.0:
                dist = 1.0
            
            dist = dist**(1.0 - halo_strength)

            r, g, b = colour
            r += 30
            g += 30
            b += 30
            r = r * luminance * (1.0 - dist)
            g = g * luminance * (1.0 - dist)
            b = b * luminance * (1.0 - dist)
            if r > 255.0: r = 255.0
            if b > 255.0: b = 255.0
            if g > 255.0: g = 255.0
            col = (int(r), int(g), int(b))
                
            if cx % block_size != 0 or cy % block_size != 0:
                if raw_dist < 0.1:
                    # draw the outline of the star, so it's less blocky
                    px(pos[0] + cx, pos[1] + cy, col)
                    px(pos[0] - cx, pos[1] + cy, col)
                    px(pos[0] + cx, pos[1] - cy, col)
                    px(pos[0] - cx, pos[1] - cy, col)
                continue


            col0, col1 = sorted([(k, coldist(col, k)) for i, k in asd.items()], key=lambda a: a[1])[0:2]
            between = round((col1[1]-col0[1])/(col0[1] + col1[1]) * (block_size**2 - 1))

            block = dithers[between]
            cols = col1[0], col0[0]
            for dx in range(block_size):
                for dy in range(block_size):
                    xa, ya = cx + dx, cy + dy
                    colo = cols[block[dx][dy]]

                    px(pos[0] + xa, pos[1] + ya, colo)
                    px(pos[0] - xa, pos[1] + ya, colo)
                    px(pos[0] + xa, pos[1] - ya, colo)
                    px(pos[0] - xa, pos[1] - ya, colo)


    ray_radius = halo_radius ** 1.4 / 2
    # print(luminance)
    for cx in range(0, int(ray_radius)):
        width = max(int((ray_radius - cx) / 12),1)
        for cy in range(0, width):
            x = cx + pos[0]
            y = cy + pos[1]
            
            raw_dist = sqrt((cx)**2 + (cy)**2) - radius
            dist = raw_dist / ray_radius
            dist /= 1.5 # stronger, for rays

            if dist < 0.0:
                dist = 0.0
            if dist > 1.0:
                dist = 1.0
            
            dist = dist**(1.0 - halo_strength)

            r, g, b = colour
            r += 30
            g += 30
            b += 30
            r = r * luminance * (1.0 - dist)
            g = g * luminance * (1.0 - dist)
            b = b * luminance * (1.0 - dist)
            if r > 255.0: r = 255.0
            if b > 255.0: b = 255.0
            if g > 255.0: g = 255.0
            col = (int(r), int(g), int(b))
            
            # closest in the palette
            col = min([(k, coldist(col, k)) for i, k in asd.items()], key=lambda a: a[1])[0]
            
            # h rays, reflected
            px(pos[0] + cx, pos[1] + cy, col)
            px(pos[0] - cx, pos[1] + cy, col)
            px(pos[0] - cx, pos[1] - cy, col)
            px(pos[0] + cx, pos[1] - cy, col)

            # v rays, reflected
            px(pos[0] + cy, pos[1] + cx, col)
            px(pos[0] - cy, pos[1] + cx, col)
            px(pos[0] - cy, pos[1] - cx, col)
            px(pos[0] + cy, pos[1] - cx, col)

def main():
    global add
    temp = 4500
    radius = 6
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    temp *= 1.1
                    add = True
                if event.key == pygame.K_RIGHT:
                    temp /= 1.1
                    add = False

                if event.key == pygame.K_UP:
                    radius += 1
                if event.key == pygame.K_DOWN:
                    radius -= 1

                if radius < 1: radius = 1
                # print(temp,radius)
                    

        for x in range(W):
            for y in range(H):
                buf[x][y] = (0, 0, 0)

        # for temp, x in reversed([(300*1.9**t + 500, int(W/10 * (t + 0.5))) for t in range(10)]):
        #     for rad, y in reversed([(t * 1 + 1, int(H/7 * (t/5 + 0.1) * (t + 0.5)) + 10) for t in range(6)]):
        #         draw_star((x, y), temp, rad)
        # for i in range(150):
        #     random.seed((i + 1)*100*radius**2)
        #     min_temp = 400
        #     max_temp = 30000
        #     exp = 3
        #     draw_star((random.randint(10, W - 10), random.randint(10, H - 10)), random.randint(int(min_temp**(1/exp)), int(max_temp**(1/exp)))**exp, random.random() ** 6.5 * 6)
        draw_star((W//2, H//2), 2000, 5)
        
        for x in range(W):
            for y in range(H):
                pygame.draw.rect(screen, buf[x][y], (x * scl, y * scl, scl, scl))
        pygame.display.flip()


main()
