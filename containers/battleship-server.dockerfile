# 1st container to compile the app
FROM debian:bookworm-slim AS builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    git \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

# Clone the repository and build the application
WORKDIR /app
RUN git clone https://github.com/RemiNeveu/c_battleship.git .

WORKDIR /app/server
RUN make build

# 2nd container to run the app
FROM debian:bookworm-slim

# Copy the built application from the builder stage
WORKDIR /app
COPY --from=builder /app/server/build/battleship-server /app/battleship-server

# Create a non-root user
RUN useradd -m appuser

# Switch to the non-root user
USER appuser

# Set a default port value using an environment variable
ENV SERVER_PORT=50000


# Run the battleship-server with the configurable port
CMD ./battleship-server "$SERVER_PORT"
# CMD ["./battleship-server", "50000"]


# Docker or podman

# Build the image
# docker build -f battleship-server.dockerfile -t battleship-server-image .

# Run the image on the default port (50000)
# docker run --name battleship-server -p 50000:50000 battleship-server-image

# Run the image on a custom port
# docker run --name battleship-server -p 40000:40000 -e SERVER_PORT=40000 battleship-server-image
