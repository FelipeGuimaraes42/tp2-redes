To compile this program, you only have to use the command 'make' in the terminal

After compiling it, you have to initialize the server first with the command ./server <protocol> <port>

In another terminal tab, after the server is up, use the command ./client <host> <port> start
    to start the client. You must use the command 'start' after the host and port, because it will start the game

After that, the client sends messages to the servers and receives an answer. This answer is always displayed.

The servers can receive the following commands:
    getdefenders
        this command shows to the player the defenders positions
    getturn x
        this command starts the turn, spawns Pokémon and make them walk if x > 0
        this command also makes the servers send to the client a full attackers position info
        the info is:
            the base, numbered one to four
            the turn, zero to forty nine
            fixed location, one to four
            and the info of a Pokémon a b c
                a is the id
                b is the name
                c is how many shots it has taken
        x must be a natural number or zero
        x must follow an order, i.e., if the user tries to send 1 before 0, the game closes
    shot x y z
        this command makes the defender Pokémon shots in an attacker Pokémon
        x is the row that the defender Pokémon is
        y is the column that the defender Pokémon is
        z is the id of the attacker Pokémon
    quit breaks the connection
    every other command sent to the servers is answered with the game over message
        the game over message is game over i j k
            i is the successfully hits that defenders Pokémon did
            j is how many Pokémon reached the Pokédex
            k is the time that it took