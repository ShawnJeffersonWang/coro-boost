# CoroBoost

CoroBoost is a partial server framework developed from scratch in C++ for Linux environments. It primarily implements a coroutine library based on `ucontext_t`, incorporating an N-M coroutine scheduler that utilizes epoll and timers. The framework supports callback for IO events and timer events, and it provides asynchronous execution by hooking and wrapping common system APIs based on coroutine scheduling. Additionally, CoroBoost includes logging, configuration functionalities, multithreading, and lock encapsulations.

## Features

- **Coroutine Library**: Implemented using `ucontext_t` for asymmetric coroutines, allowing switching between child coroutines and the main thread coroutine.
- **Scheduler**: An N-M coroutine scheduler based on `epoll` and timers, supporting the scheduling of both timed task coroutines and IO task coroutines. The main thread (the thread that creates the scheduler) can also participate in scheduling.
- **Timer**: A timer feature based on a time heap, supporting the addition, deletion, and updating of timed events.
- **Hooks**: Wrapped blocking system calls such as `sleep` and IO operations with hooks to convert them into non-blocking calls using coroutine switching.
- **Multithreading and Locks**: Encapsulated `pthread`, mutexes, semaphores, read-write locks, spinlocks, and implemented range locks.
- **Logging and Configuration**: Comprehensive logging and configuration capabilities.

## Key Concepts

- **Coroutines**: Deep understanding of coroutine technology, including shared stacks, symmetric/asymmetric coroutines.
- **IO Multiplexing**: Familiarity with IO multiplexing and event-driven models.
- **System Programming**: Proficient in Linux network programming and system programming interfaces.

## Getting Started

### Prerequisites

- Linux operating system
- C++ compiler (e.g., g++)
- Development tools and libraries (e.g., make, pthread)

### Building CoroBoost

1. Clone the repository:
    ```sh
    git clone https://github.com/ShawnJeffersonWang/CoroBoost.git
    cd CoroBoost
    ```

2. Build the project:
    ```sh
    make
    ```

### Running Examples

Refer to the examples directory for sample applications demonstrating the usage of CoroBoost features.

## Documentation

Detailed documentation is available in the `docs` directory. It includes a comprehensive guide on using CoroBoost, along with API references and examples.

## Contributing

Contributions are welcome! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for details on the code of conduct, and the process for submitting pull requests.

## License

CoroBoost is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.
