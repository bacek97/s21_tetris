# BrickGame Racing (Go)
Summary: In this project, you need to implement the Racing game in the Go programming language.

💡 [Tap here](https://new.oprosso.net/p/4cb31ec3f47a4596bc758ea1861fb624) **to leave your feedback on the project**. It's anonymous and will help our team make your educational experience better. We recommend completing the survey immediately after the project.

# Contents

- [BrickGame Races (Go)](#brickgame-racing)
  - [Contents](#contents)
  - [Introduction](#introduction)
- [Chapter I](#chapter-i)
  - [General Information](#general-information)
    - [Races](#races)
    - [HTTP](#http)
    - [REST API](#rest-api)
    - [Application Delivery](#application-delivery)
    - [Support for interfaces from previous parts of BrickGame](#support-for-interfaces-from-previous-parts-of-brickgame)
  - [Chapter II](#chapter-ii)
  - [Project Requirments](#project-requirements)
    - [Part 1. Main task](#part-1-main-task)
    - [Part 2. Bonus. Scoring and game record](#part-2-bonus-scoring-and-game-record)
    - [Part 3. Bonus. Level mechanics](#part-3-bonus-level-mechanics)

## Introduction

The implementation of the Racing game in the Go programming language will consist of the development of several individual components: a library responsible for implementing the game logic, a web interface and a server providing REST API for accessing games via the network.

The developed library also needs to be connected to the console interface from BrickGame v1.0 and the desktop interface from BrickGame v2.0. Both interfaces must fully support the new game.

Games from the BrickGame v1.0 and BrickGame v2.0 projects must be accessible via the developed REST API.

This project provides a REST API specification that needs to be implemented and a web interface that supports the specification. You can find these materials in the `materials` and `code-samples` folders.

## Chapter I 
# General information

Remember to use finite-state machines to formalize the logic of the game. You can find descriptions and examples of FMS to refresh your knowledge in `materials/brick-game-v1.0.md`.

### Races

![Races](misc/images/race-game.png)

The game was based on a 1974 arcade game produced by Taito called Speed Race. That arcade game had a physical steering wheel with which the player drove the car on the track. The concept for this game was later adapted for the BrickGame console.

In this game, the user controls a racing car that moves on a track divided into three lanes. The game constantly generates rival cars moving along the track. The player needs to avoid collisions with them by changing lanes. The generated cars appear at the upper border of the playing field and gradually shift downwards without changing the lane. If you hit that car, the game is over.

### HTTP

HTTP (HyperText Transfer Protocol), a protocol for transferring hypertext documents, is designed for client-server interaction, where the client and server exchange messages. Messages are divided into two types: requests and responses. Requests are sent by the client, responses are sent by the server.

A request consists of the method used, headers, and an optional request body containing the passed data. Each method is intended to perform a specific action on the server. The main ones are:

- `GET` to retrieve the data;
- `POST` to send the data;
- `DELETE` to delete the data;
- `PATCH` to edit the data.

When you send data, it is placed in the body of the request, and the transmission format can be different: regular text, XML, JSON, etc. JSON is mostly used because of its readability and support in JavaScript.

The response consists of the response code, headers, and request body. Headers and request body are used in the same way as in requests. The response code, on the other hand, is intended for sending information about the request result to the client. They consist of three digits and are divided into five groups. The codes of each group start with a different digit:

- informational (100-199),
- successful (200-299),
- redirection (300 – 399),
- client errors (400-499)
- server errors (500-599)

### REST API

API (Application Programming Interface) is intended to formalize the ways of interaction between programs.

REST API is an architectural approach to building Web APIs. In order to design a REST API, you need to identify all the resources to which you will be granted access. After that, you need to identify all available operations for each resource and logically group them into routes using different methods, parameters and filters.

For example, let's take a library website. We want to provide a searchable list of available books by author. Then we would have two resources: the book and the author. We will have only one route for an author – getting a list of all authors. There are two routes for books: getting a list of all books and getting information about a specific book.

- `server_url/api/authors` — the route to get a list of authors;
- `server_url/api/books?author_id=<id>` — the route to get a list of books, where `author_id` is a filter to get books of only the given author. The filter can be omitted, then a list of all books will be given;
- `server_url/api/books/<id>` — getting information about a particular book by identifier. `id` - route parameter.

As you can see above, resources in routes are always plural.

By applying HTTP methods, you can use the same route for different tasks. Using the route `server_url/api/books/<id>` and the `GET` method, you can request information on a specific book. If the server allows you to delete resources, then the same route can be used for deletion, only instead of the `GET` method there will be a `DELETE` method.

### Application Delivery

You must first download the application to run it in a browser. This requires configuring the distribution of static files (html, css, js) on the server. There are two ways: we can distribute static using our server application, or we can set up a separate server to distribute static. The first approach increases the load on the server application, which can be a problem in real projects, but it simplifies the task somewhat, since in most languages it can be configured by adding a few lines of code. The second approach allows unloading the server application and speeding up the distribution of static files, since it uses applications specially designed for this purpose (e.g., Nginx), but requires additional configuration.

### Support for interfaces from previous parts of BrickGame

Connect the game library via REST API. You need to develop a small client library for the console and desktop interface that encapsulates the API call and provides a universal interface.

## Chapter II 
## Project Requirements

### Part 1. Main task

Implement BrickGame v3.0:

- The server must be developed in Go.
- It is necessary to use finite-state machines for formalizing game logic.
- Implement a REST API for client-server interaction. It must conform to the specification from the `materials/rest-api-specification.yaml` file.
- The game logic must be implemented in a library, which must be placed in the `src/brick_game/race` folder.
- The server code must be in the `src/brick_game/server` folder.
- The code for the web interface must be placed in the `src/brick_game/web_gui` folder (you can use the ready-made interface from the `code-samples/web-gui` folder).
- Stick to Effective Go when writing code.
- The library that implements the game logic must be covered by unit tests using `testify` framework. - Pay special attention to checking the FMS states and transitions. The coverage of the library with game logic with tests must be at least 80 percent.
- Set up static file distribution.
- The following mechanics must be in the racing game:
  - The player's car can change lanes by pressing the left/right arrows;
  - If you collide with another car, the game ends;
  - Rival cars appear at the top edge of the playing field and gradually shift downward;
  - When you hold the forward arrow, rival cars shift at an increased speed that is displayed in the user interface.
- Copy the library folder with the game logic from the BrickGame v1.0 project.
- Copy the library folder with the game logic from the BrickGame v2.0 project.
- Copy the folder with the console interface of the game from the BrickGame v1.0 project.
- Copy the folder with the console interface of the game from the BrickGame v2.0 project.
- The server must support games from previous BrickGame projects.
- The console interface must support the Racing game.
- The desktop interface must support the Racing game.
- Develop a client library that encapsulates the interaction with the server to interact with the REST API. This library must be used to support the game in the console and desktop clients. It must provide the universal interface described in `materials/library-specification.md`.
- The playing field is 10 "pixels" wide and 20 "pixels" high.
- The game library must have a universal interface described in the `materials/library-specification.md` file.
- Prepare a diagram showing all states and transitions between them for the implemented FMS for project submission.

### Part 2. Bonus. Scoring and game record

Add the following mechanics to the game:

- scoring;
- storing maximum points.

This information must be passed and displayed by the user interface in the sidebar. The maximum number of points must be stored in a file or embedded DBMS and saved between program runs.

The maximum number of points must be changed during the game if the user exceeds the current maximum score.

Points will be accrued as follows:

### Part 3. Bonus. Level mechanics

Add level mechanics to the game. Each time a player gains 5 points, the level increases by 1. Increasing the level boosts the speed of cars. The maximum number of levels is 10.
