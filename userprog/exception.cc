// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which) {
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
        case SyscallException:
            switch (type) {
                case SC_Exit:
                {
                    DEBUG(dbgSys, "Exit: Thread ID" << kernel->machine->ReadRegister(4) << "\n");
                    int status = (int) kernel->machine->ReadRegister(4);
                    // ThreadId id = (int)kernel->currentThread->PID;
                    // Exit_POS(id);
                    //printf("Thread %s exited with status: %d\n",currentThread->getName().c_str(),status);
                    //cout << "Thread with PID " << kernel->currentThread->PID << " is going to finish! with status: " << status << endl;
                    kernel->currentThread->Finish();
                    ASSERTNOTREACHED();
                }
                    break;
                case SC_Halt:
                    DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

                    SysHalt();

                    ASSERTNOTREACHED();
                    break;

                case SC_Add:
                    DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

                    /* Process SysAdd Systemcall*/
                    int result;
                    result = SysAdd(/* int op1 */(int) kernel->machine->ReadRegister(4),
                            /* int op2 */(int) kernel->machine->ReadRegister(5));

                    DEBUG(dbgSys, "Add returning with " << result << "\n");
                    /* Prepare Result */
                    kernel->machine->WriteRegister(2, (int) result);

                    /* Modify return point */
                {
                    /* set previous programm counter (debugging only)*/
                    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                    /* set next programm counter for brach execution */
                    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                }

                    return;

                    ASSERTNOTREACHED();

                    break;
                case SC_Create:
                {
                    int addr = kernel->machine->ReadRegister(4);
                    int protection = kernel->machine->ReadRegister(5);
                    int size = kernel->machine->ReadRegister(6);
                    int i;

                    //reading file name from memory
                    char* buffer = (char*) malloc(100 * sizeof (char));
                    for (i = 0;; i++) {
                        kernel->machine->ReadMem(addr + i, 1, (int*) &buffer[i]);
                        if (buffer[i] == '\0')
                            break;
                    }

                    printf("Creating file %s\n", buffer);

                    if (kernel->fileSystem->Create(buffer, size, protection))
                        kernel->machine->WriteRegister(2, 0);
                    else
                        kernel->machine->WriteRegister(2, -1);

                    delete buffer;

                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;

                case SC_Remove:
                {
                    int addr = kernel->machine->ReadRegister(4);
                    int i;

                    //reading file name from memory
                    char* buffer = (char*) malloc(100 * sizeof (char));
                    for (i = 0;; i++) {
                        kernel->machine->ReadMem(addr + i, 1, (int*) &buffer[i]);
                        if (buffer[i] == '\0')
                            break;
                    }

                    printf("Removing file %s\n", buffer);

                    if (kernel->fileSystem->Remove(buffer)) {
                        kernel->machine->WriteRegister(2, 0);
                    } else
                        kernel->machine->WriteRegister(2, -1);

                    delete buffer;

                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;

                case SC_Open:
                {
                    int name = kernel->machine->ReadRegister(4);
                    int mode = kernel->machine->ReadRegister(5);

                    //reading file name from memory
                    char* fileName = (char*) malloc(100 * sizeof (char));
                    for (int i = 0;; i++) {
                        kernel->machine->ReadMem(name + i, 1, (int*) &fileName[i]);
                        if (fileName[i] == '\0')
                            break;
                    }



                    OpenFile* of = kernel->fileSystem->Open(fileName, mode);

                    if (of == NULL) {
                        printf("\nCannot open file %s\n", fileName);
                        kernel->machine->WriteRegister(2, -1);
                    } else
                        kernel->machine->WriteRegister(2, of->getId());

                    printf("Opening file %s with id %d\n", fileName, of->getId());

                    
                    delete fileName;

                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;

                case SC_Write:
                {
                    int addr = kernel->machine->ReadRegister(4);
                    int size = kernel->machine->ReadRegister(5);
                    int fileId = kernel->machine->ReadRegister(6);
                    int i, buffer;

                    char *_buffer = new char;

                    for (i = 0; i < size; i++) {
                        kernel->machine->ReadMem(addr, 1, &buffer);
                        _buffer[i] = (char) buffer;
                        addr++;
                    }
                    printf("Writing \"%s\" to the file with id %d\n", _buffer, fileId);
                    OpenFile *file = kernel->findOpenFileById(fileId);
                    int res = file->Write(_buffer, size);
                    kernel->machine->WriteRegister(2, res);



                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;

                case SC_Read:
                {
                    int addr = kernel->machine->ReadRegister(4);
                    int size = kernel->machine->ReadRegister(5);
                    int fileId = kernel->machine->ReadRegister(6);
                    printf("FILEID %d\n", fileId);
                    char *buffer = new char;

                    OpenFile *file = kernel->findOpenFileById(fileId);
                    int res = file->Read(buffer, size);
                    printf("Reading \"%s\" from file with id %d\n", buffer, file->getId());

                    kernel->machine->WriteRegister(2, res);


                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;

                case SC_Seek:
                {
                    int pos = kernel->machine->ReadRegister(4);
                    int fileId = kernel->machine->ReadRegister(5);
                    printf("FILEID %d\n", fileId);

                    OpenFile *file = kernel->findOpenFileById(fileId);
                    file->Seek(pos);
                    printf("Seeking to %d in file with id %d\n", pos, file->getId());

                    kernel->machine->WriteRegister(2, 1);


                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;

                case SC_Close:
                {
                    int fileId = kernel->machine->ReadRegister(4);
                    char *buffer = new char;

                    kernel->closeOpenFileById(fileId);
                    kernel->currentThread->closeOpenFileById(fileId);
                    printf("Closing file %d\n", fileId);
                    kernel->machine->WriteRegister(2, 0);


                    {
                        /* set previous programm counter (debugging only)*/
                        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

                        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
                        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

                        /* set next programm counter for brach execution */
                        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
                    }
                    return;
                    ASSERTNOTREACHED();
                }
                    break;
                default:
                    cerr << "Unexpected system call " << type << "\n";
                    break;
            }
            break;
        default:
            cerr << "Unexpected user mode exception" << (int) which << "\n";
            break;
    }
    ASSERTNOTREACHED();
}
