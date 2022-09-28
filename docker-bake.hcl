group "default" {
    targets = [
        "python-36-linux",
        "python-37-linux",
        "python-38-linux",
        "python-39-linux",
        "python-310-linux"
    ]
}

target "linux" {
    context = "./"
    dockerfile = "docker/python-linux/Dockerfile"
}

target "python-36-linux" {
    inherits = [ "linux" ]
    tags = [ "python-36-linux" ]
    args = {
        PYTHON_TAG = "3.6"
    }
    platforms = [ "linux/amd64", "linux/arm64/v8" ]
}

target "python-37-linux" {
    inherits = [ "linux" ]
    tags = [ "python-37-linux" ]
    args = {
        PYTHON_TAG = "3.7"
    }
    platforms = [ "linux/amd64", "linux/arm64/v8" ]
}

target "python-38-linux" {
    inherits = [ "linux" ]
    tags = [ "python-38-linux" ]
    args = {
        PYTHON_TAG = "3.8"
    }
    platforms = [ "linux/amd64", "linux/arm64/v8" ]
}

target "python-39-linux" {
    inherits = [ "linux" ]
    tags = [ "python-39-linux" ]
    args = {
        PYTHON_TAG = "3.9"
    }
    platforms = [ "linux/amd64", "linux/arm64/v8" ]
}

target "python-310-linux" {
    inherits = [ "linux" ]
    tags = [ "python-310-linux" ]
    args = {
        PYTHON_TAG = "3.10"
    }
    platforms = [ "linux/amd64", "linux/arm64/v8" ]
}
