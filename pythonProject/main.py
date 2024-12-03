import pygame
import time


def read_graph_file(filename):
    """Reads the graph file and returns a list of frames (each frame is a 2D grid of characters)."""
    frames = []
    current_frame = []

    with open(filename, 'r') as file:
        for line in file:
            line = line.rstrip()  # Remove the newline character at the end of the line

            # If the line is a separator (dashes), store the current frame and start a new one
            if '-' in line:
                if current_frame:
                    frames.append(current_frame)
                    current_frame = []
            else:
                current_frame.append(line)

        # Add the last frame if there's any left
        if current_frame:
            frames.append(current_frame)

    return frames


def display_frames(frames, screen, width, height):
    """Displays the frames one by one to show the movement of '*' in a graphical window."""
    font = pygame.font.SysFont('Arial', 12)  # Create a font for rendering text

    for frame in frames:
        # Clear the screen with a white color
        screen.fill((255, 255, 255))

        # Draw the current frame
        for y, line in enumerate(frame):
            for x, char in enumerate(line):
                if char == '*':
                    # Draw red circle at '*' position
                    pygame.draw.circle(screen, (255, 0, 0), (x * 8, y * 8), 4)  # Adjusted for 100x100 grid

        # Update the display
        pygame.display.update()

        # Pause for a moment to simulate the movement
        time.sleep(0.1)


def find_star_position(frame):
    """Finds the position of '*' in a given frame (returns row, column)."""
    for i, line in enumerate(frame):
        if '*' in line:
            return i, line.index('*')
    return None, None


def main():
    # Initialize Pygame
    pygame.init()

    # Read the graph file and get all frames
    filename = 'graph.txt'  # Make sure your graph.txt file is in the same directory
    frames = read_graph_file(filename)

    # Set up the display window size (100x100 grid scaled to 800x800 window)
    width, height = 800, 800  # Adjusted for better visibility
    screen = pygame.display.set_mode((width, height))
    pygame.display.set_caption("Movement of '*'")

    # Display the movement of '*' from frame to frame
    print("Displaying the movement of '*' from first frame to last...\n")
    display_frames(frames, screen, width, height)

    # For additional debugging, we can print the position of '*' in each frame
    print("\nPositions of '*' in each frame:")
    for index, frame in enumerate(frames):
        row, col = find_star_position(frame)
        if row is not None and col is not None:
            # Corrected the f-string
            print(f"Frame {index + 1}: Position of '*' is at ({row}, {col})")

    # Quit Pygame
    pygame.quit()


if __name__ == "__main__":
    main()
