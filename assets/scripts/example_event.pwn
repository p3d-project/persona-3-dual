#include <core>
#include <console>

forward OnPlayerInteract(eventId);

public OnPlayerInteract(eventId)
{
    printf("Player interacted with event!\n", eventId);
    return 1;
}

main()
{
    print("Example event script loaded.\n");
}
