#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define RUNS 2
#define PUT_DOWN_LEFT 6
#define PUT_DOWN_RIGHT 5
#define PICKED_UP_LEFT 4
#define PICKED_UP_RIGHT 3
#define THINKING 2
#define HUNGRY 1
#define EATING 0

// int state[5]; //keeps track of
int philosphers[5]; // to keep track of each philosphers state
int phil[5] = {0, 1, 2, 3, 4};

sem_t mutex; // ensures only one function can go at once
sem_t chopsticks[5];
sem_t room;

void *print_states(void *x)
{
    sem_wait(&mutex);
    // printf("<>");
    for (int i = 0; i < 5; ++i)
    {
        if (philosphers[i] == HUNGRY)
        {
            printf("|      HUNGRY     |");
        }
        else if (philosphers[i] == EATING)
        {
            printf("|      EATING     |");
        }
        else if (philosphers[i] == PICKED_UP_LEFT)
        {
            printf("|  PICKED_UP_LEFT |");
            philosphers[i] = THINKING;
        }
        else if (philosphers[i] == PICKED_UP_RIGHT)
        {
            printf("| PICKED_UP_RIGHT |");
            philosphers[i] = THINKING;
        }
        else if (philosphers[i] == THINKING)
        {
            printf("|    THINKING     |");
        }
        else if (philosphers[i] == PUT_DOWN_LEFT)
        {
            printf("|  PUT_DOWN_LEFT  |");
            philosphers[i] = THINKING;
        }
        else if (philosphers[i] == PUT_DOWN_RIGHT)
        {
            printf("|  PUT_DOWN_RIGHT |");
            philosphers[i] = THINKING;
        }
        else
        {
            printf("|      ERROR      |");
        }
    }
    printf("\n");
    printf("+=================||=================||=================||=================||==================+\n");
    sem_post(&mutex);
    return NULL;
}

int want_eat(int x)
{
    // change state to hungry
    // call the change state funtion that prints all states
    // check if there is room
    // if yes room.wait()
    // pick up left chopstick then right
    // change state to eating
    // call change state function
    //sem_wait(&mutex);
    // printf("Want_eat was called by %d\n", x);
    philosphers[x] = HUNGRY;
    print_states(NULL);
    int test_num;
    int chop_num;
    sem_getvalue(&room, &test_num);
    //printf("Test_num is %d\n", test_num);
    if (test_num != 0) {
        // for (int z = 0; z < 5; ++z) {
        //     sem_getvalue(&chopsticks[z], &chop_num);
        //     printf("<><><>%d<><><>\n", chop_num);
        // }
        sem_wait(&room);
        sem_wait(&chopsticks[x]);
        // printf("\n<><>%d<><>\n", test_num);
        philosphers[x] = PICKED_UP_LEFT;
        print_states(NULL);
        //sem_wait(&chopsticks[(x + 4) % 5]);
        philosphers[x] = PICKED_UP_RIGHT;
        print_states(NULL);
        philosphers[x] = EATING;
        print_states(NULL);
    } else {
        //sem_post(&mutex);
        philosphers[x] = HUNGRY;
        print_states(NULL);
        //printf("sending 1\n");
        return 1;
    }

    //sem_post(&mutex);
    return 0;
}

void *done_eat(int x)
{
    //sem_wait(&mutex);
    // printf("Done_eat was called by %d\n", x);
    sem_post(&chopsticks[(x + 4) % 5]);
    philosphers[x] = PUT_DOWN_RIGHT;
    print_states(NULL);
    //philosphers[x] = THINKING;
    sem_post(&chopsticks[x]);
    philosphers[x] = PUT_DOWN_LEFT;
    print_states(NULL);
    sem_post(&room);
    philosphers[x] = THINKING;
    print_states(NULL);

    //sem_post(&mutex);
    return NULL;
}

void *philospher(void *x)
{
    // int runs = RUNS;
    int *i = x;
    for (int j = 0; j < RUNS; ++j)
    {
        int w = want_eat(*i);
        sleep(2);
        if (w != 1)
        {
            //printf("recived 1 value\n");
            // want_eat(*i);
            // sleep(2);
            // done_eat(*i);
            // sleep(2);
            //++j;
            //done_eat(*i);
        }
        while (w == 1) {
            w = want_eat(*i);
            sleep(3);
        }
        done_eat(*i);
        sleep(2);
    }

    return NULL;
}

int main(int argc, const char *argv[])
{
    pthread_t phil_thread[5]; // array of philospher threads
    // initialize the semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&room, 0, 4);
    printf("+=================||=================||=================||=================||==================+\n");
    printf("|                         ------>    DINING    PHILOSOPHERS   <------                         |\n");
    printf("+=================||=================||=================||=================||==================+\n");
    printf("|      Phil_0     ||      Phil_1     ||      Phil_2     ||      Phil_3     ||     Phil_4      ||\n");
    printf("+=================||=================||=================||=================||==================+");

    for (int i = 0; i < 5; ++i)
    {
        sem_init(&chopsticks[i], 0, 1);
    }
    for (int i = 0; i < 5; ++i)
    {
        pthread_create(&phil_thread[i], NULL, philospher, &phil[i]);
        // printf("<>%d is thinking<>", i);
        philosphers[i] = THINKING;
    }
    printf("\n");
    for (int i = 0; i < 5; ++i)
    {
        pthread_join(phil_thread[i], NULL);
        // printf("<>just joined<>\n");
    }
    for (int i = 0; i < 5; ++i)
    {
        sem_destroy(&chopsticks[i]);
        // Starts as 0 because they can't eat when we start
    }
    return 0;
}
//============OUTPUT FOR TWO RUNS============
//osc@ubuntu:~/final-src-osc10e/ch5$ ./DiningPhil
// +=================||=================||=================||=================||==================+
// |                         ------>    DINING    PHILOSOPHERS   <------                         |
// +=================||=================||=================||=================||==================+
// |      Phil_0     ||      Phil_1     ||      Phil_2     ||      Phil_3     ||     Phil_4      ||
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      HUNGRY     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||  PICKED_UP_LEFT ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     || PICKED_UP_RIGHT ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||      HUNGRY     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||  PICKED_UP_LEFT ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     || PICKED_UP_RIGHT ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||      EATING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||      EATING     ||  PICKED_UP_LEFT |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||      EATING     || PICKED_UP_RIGHT |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||      HUNGRY     ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||  PICKED_UP_LEFT ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     || PICKED_UP_RIGHT ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||      EATING     ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||      EATING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||  PUT_DOWN_RIGHT ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||  PUT_DOWN_LEFT  ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||    THINKING     ||      EATING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||    THINKING     ||      EATING     ||  PUT_DOWN_RIGHT |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||    THINKING     ||      EATING     ||  PUT_DOWN_LEFT  |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||      EATING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||  PUT_DOWN_RIGHT ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||  PUT_DOWN_LEFT  ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |  PICKED_UP_LEFT ||    THINKING     ||    THINKING     ||  PUT_DOWN_RIGHT ||    THINKING     |
// +=================||=================||=================||=================||==================+
// | PICKED_UP_RIGHT ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||  PUT_DOWN_LEFT  ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||      HUNGRY     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      HUNGRY     ||  PICKED_UP_LEFT ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      HUNGRY     || PICKED_UP_RIGHT ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      HUNGRY     ||      EATING     ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      HUNGRY     ||      EATING     ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||  PICKED_UP_LEFT ||      EATING     ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     || PICKED_UP_RIGHT ||      EATING     ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||    THINKING     ||      HUNGRY     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||    THINKING     ||  PICKED_UP_LEFT |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||    THINKING     || PICKED_UP_RIGHT |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||    THINKING     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||      EATING     ||      EATING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |  PUT_DOWN_RIGHT ||      EATING     ||      EATING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |  PUT_DOWN_LEFT  ||      EATING     ||      EATING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||      EATING     ||      EATING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||      EATING     ||  PUT_DOWN_RIGHT ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||      EATING     ||  PUT_DOWN_LEFT  ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||      EATING     ||    THINKING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||  PUT_DOWN_RIGHT ||    THINKING     ||      HUNGRY     ||      EATING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||  PUT_DOWN_LEFT  ||    THINKING     ||      HUNGRY     ||  PUT_DOWN_RIGHT |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||      HUNGRY     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||      HUNGRY     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||      HUNGRY     ||  PUT_DOWN_LEFT  |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||      HUNGRY     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||      HUNGRY     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||  PICKED_UP_LEFT ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     || PICKED_UP_RIGHT ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      HUNGRY     ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |  PICKED_UP_LEFT ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// | PICKED_UP_RIGHT ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||      EATING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||  PUT_DOWN_RIGHT ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||  PUT_DOWN_LEFT  ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |      EATING     ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |  PUT_DOWN_RIGHT ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |  PUT_DOWN_LEFT  ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
// |    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     ||    THINKING     |
// +=================||=================||=================||=================||==================+
