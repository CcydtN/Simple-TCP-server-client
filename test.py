import asyncio
from asyncio.subprocess import PIPE
from sys import stdout

debug_folder = "builddir/debug/"
server = debug_folder+"server"
client = debug_folder+"client"

async def main():
    tasks = []
    tmp = await asyncio.create_subprocess_exec(server)
    for i in range(1, 20):
        tasks.append(await asyncio.create_subprocess_exec(client, '127.0.0.1', '8080', str(i), stdout=PIPE, stderr=PIPE))
    while True:
        await asyncio.sleep(1)

if __name__=="__main__":
    asyncio.run(main())
