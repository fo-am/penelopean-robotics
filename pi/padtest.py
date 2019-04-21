import curses

def main(stdscr):
    stdscr.keypad(True)
    stdscr.refresh() # This is the necessary initial refresh

    heigth, width = 20, 15 # size of the pad
    top, left = 10, 20 # where to place pad
    viewportHeight = 10
    scrollOffset = 0

    # generate a string to fill pad
    string = ''.join(chr(ord('a') + (x % 26)) for x in range(heigth*width-1))

    pad = curses.newpad(heigth, width)
    pad.addstr(string)
    pad.refresh(scrollOffset, 0, top, left, top + 10, left + width)

    cmd = stdscr.getch()
    while True:
        if  cmd == curses.KEY_DOWN and scrollOffset < heigth - viewportHeight - 1:
            scrollOffset += 1
        elif cmd == curses.KEY_UP and scrollOffset > 0:
            scrollOffset -= 1
        if cmd:
            pad.refresh(scrollOffset, 0, top, left, top + 10, left + width)
        cmd = stdscr.getch()

curses.wrapper(main)
