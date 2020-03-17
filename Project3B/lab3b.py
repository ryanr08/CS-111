#!/usr/bin/env python3
#NAME: Anurag Sandireddy, Ryan Riahi
#EMAIL: anurag.sandireddy@gmail.com, ryanr08@gmail.com
#ID: 805114200, 105138860


import sys
import os
import re


def findErrors():
    if len(sys.argv) != 2:
        print("Incorrect number of arguments", file=sys.stderr)
        quit(1)
    try:
        fd = open(sys.argv[1], "r")
    except:
        print("Second argument must be a valid file", file=sys.stderr)
        quit(1)
    contents = fd.read()
    global inconsistency
    inconsistency = False
    global superblock
    global groups
    global freeBlocks
    global freeInodes
    global inodes
    global dirents
    global indirects
    global allInodes
    superblock = re.findall('SUPERBLOCK.*', contents)
    groups = re.findall('GROUP.*', contents)
    freeBlocks = re.findall("BFREE.*", contents)
    freeInodes = re.findall("IFREE.*", contents)
    dirents = re.findall("DIRENT.*", contents)
    inodes = re.findall("INODE.*", contents)
    indirects = re.findall("INDIRECT.*", contents)
    blockConsistency()
    inodeAudits()
    directoryConsistency()
    if inconsistency:
        quit(2)
    else:
        quit(0)


def blockConsistency():
    highestBlock = int(superblock[0].split(',')[1])
    for inode in inodes:
        inodeNum = inode.split(',')[1]
        blocks = inode.split(',')[12:24]
        for block in blocks:
            if (int(block) < 0) or (int(block) > highestBlock):
                print("INVALID BLOCK " + block + " IN INODE " +
                      inodeNum + " AT OFFSET " + str(blocks.index(block)))
                inconsistency = True
            elif (int(block) <= 3 & int(block) > 0):
                print("RESERVED BLOCK " + block + " IN INODE " +
                      inodeNum + " AT OFFSET " + str(blocks.index(block)))
                inconsistency = True
        indirect = inode.split(',')[24:27]
        for indi in indirect:
            level = str(1+indirect.index(indi))
            offset = str(indirect.index(indi) + 12)
            strlevel = "INDIRECT"
            if int(level) == 2:
                strlevel = "DOUBLE INDIRECT"
                offset = "268"
            if int(level) == 3:
                strlevel = "TRIPLE INDIRECT"
                offset = "65804"
            if int(indi) < 0 or int(indi) > highestBlock:
                print("INVALID " + strlevel + " BLOCK " + indi
                      + " IN INODE " + str(inodeNum) + " AT OFFSET " + offset)
                inconsistency = True
            if (int(indi) <= 3 and int(indi) > 0):
                print("RESERVED " + strlevel + " BLOCK " + indi
                      + " IN INODE " + str(inodeNum) + " AT OFFSET " + offset)
                inconsistency = True
    firstBlock = int(int(groups[0].split(',')[8]) + int(groups[0].split(',')[3])
                     * int(superblock[0].split(',')[4]) / int(superblock[0].split(',')[3]))
    allBlocks = range(firstBlock, highestBlock)
    bfreeList = []
    for item in freeBlocks:
        bfreeList.append(int(item.split(',')[1]))
    referencedBlocks = {}
    for inode in inodes:
        blocks = inode.split(',')[12:24]
        for block in blocks:
            if int(block) > 3 and int(block) < highestBlock:
                if int(block) in referencedBlocks:
                    referencedBlocks[int(block)].extend(
                        [0, inode.split(',')[1], int(blocks.index((block)))])
                else:
                    referencedBlocks[int(block)] = [0, inode.split(',')[
                        1], int(blocks.index((block)))]
        block = inode.split(',')[24:27]
        if int(block[0]) > 3 and int(block[0]) < highestBlock:
            if int(block[0]) in referencedBlocks:
                referencedBlocks[int(block[0])].extend(
                    [1, inode.split(',')[1], int(12 + block.index((block[0])))])
            else:
                referencedBlocks[int(block[0])] = [1, inode.split(',')[
                    1], int(12 + block.index((block[0])))]
        if int(block[1]) > 3 and int(block[1]) < highestBlock:
            if int(block[1]) in referencedBlocks:
                referencedBlocks[int(block[1])].extend(
                    [2, inode.split(',')[1], 268])
            else:
                referencedBlocks[int(block[1])] = [2, inode.split(',')[
                    1], 268]
        if int(block[2]) > 3 and int(block[2]) < highestBlock:
            if int(block[2]) in referencedBlocks:
                referencedBlocks[int(block[2])].extend(
                    [3, inode.split(',')[1], 65804])
            else:
                referencedBlocks[int(block[2])] = [3, inode.split(',')[
                    1], 65804]
    for indirect in indirects:
        blockNum = int(indirect.split(',')[5])
        inodeNum = int(indirect.split(',')[1])
        level = int(indirect.split(',')[2]) - 1
        offset = int(indirect.split(',')[3])
        if level == 1:
            offset = 268
        elif level == 2:
            offset = 65804
        if blockNum > 3 and blockNum < highestBlock:
            if blockNum in referencedBlocks:
                referencedBlocks[blockNum].extend(
                    [level, inodeNum, offset])
            else:
                referencedBlocks[blockNum] = [
                    level, inodeNum, offset]
    for item in allBlocks:
        if item in referencedBlocks and item in bfreeList:
            print("ALLOCATED BLOCK " + str(item) + " ON FREELIST")
            inconsistency = True
        if item not in referencedBlocks and item not in bfreeList:
            print("UNREFERENCED BLOCK " + str(item))
            inconsistency = True
    for item in referencedBlocks:
        if (len(referencedBlocks[item]) > 3):
            i = 0
            while i < len(referencedBlocks[item]):
                if referencedBlocks[item][i] == 0:
                    print("DUPLICATE BLOCK " + str(item) + " IN INODE " + str(
                        referencedBlocks[item][i+1]) + " AT OFFSET " + str(referencedBlocks[item][i+2]))
                    inconsistency = True
                if referencedBlocks[item][i] == 1:
                    inconsistency = True
                    print("DUPLICATE INDIRECT BLOCK " + str(item) + " IN INODE " + str(
                        referencedBlocks[item][i+1]) + " AT OFFSET " + str(referencedBlocks[item][i+2]))
                if referencedBlocks[item][i] == 2:
                    inconsistency = True
                    print("DUPLICATE DOUBLE INDIRECT BLOCK " + str(item) + " IN INODE " + str(
                        referencedBlocks[item][i+1]) + " AT OFFSET " + str(referencedBlocks[item][i+2]))
                if referencedBlocks[item][i] == 3:
                    inconsistency = True
                    print("DUPLICATE TRIPLE INDIRECT BLOCK " + str(item) + " IN INODE " + str(
                        referencedBlocks[item][i+1]) + " AT OFFSET " + str(referencedBlocks[item][i+2]))
                i += 3

def inodeAudits():
    allInodes = [*range(11, int(superblock[0].split(',')[2]) + 1)]
    allInodes.append(2)
    global allocatedInodes
    allocatedInodes = []
    unallocatedInodes = []
    for inode in inodes:
        allocatedInodes.append(int(inode.split(',')[1]))
    for inode in freeInodes:
        unallocatedInodes.append(int(inode.split(',')[1]))
    for item in allInodes:
        if item in allocatedInodes and item in unallocatedInodes:
            inconsistency = True
            print("ALLOCATED INODE " + str(item) + " ON FREELIST")
        if item not in allocatedInodes and item not in unallocatedInodes:
            inconsistency = True
            print("UNALLOCATED INODE " + str(item) + " NOT ON FREELIST")

def directoryConsistency():
    allInodes = [*range(11, int(superblock[0].split(',')[2]) + 1)]
    allInodes.append(2)
    inodeReferences = {}
    for item in allInodes:
        inodeReferences[item] = 0
    parentDir = {}
    parentDir[2] = 2
    for dirent in dirents:
        if dirent.split(',')[6] != "'.'" and dirent.split(',')[6] != "'..'":
            parentDir[int(dirent.split(',')[3])] = int(dirent.split(',')[1])   
    for dirent in dirents:
        inodeNum = int(dirent.split(',')[3])
        if inodeNum < 1 or inodeNum > int(superblock[0].split(',')[2]):
            inconsistency = True
            print("DIRECTORY INODE " + dirent.split(',')[1] + " NAME "
            + dirent.split(',')[6] + " INVALID INODE " + str(inodeNum))
        elif inodeNum not in allocatedInodes:
            inconsistency = True
            print("DIRECTORY INODE " + dirent.split(',')[1] + " NAME "
            + dirent.split(',')[6] + " UNALLOCATED INODE " + str(inodeNum))
        else:
            #if inodeNum in inodeReferences:
            inodeReferences[inodeNum]+=1
            # elif inodeNum not in inodeReferences:
            #     inodeReferences[inodeNum] = 1
        if dirent.split(',')[6] == "'.'":
            if dirent.split(',')[3] != dirent.split(',')[1]:
                inconsistency = True
                print("DIRECTORY INODE " + dirent.split(',')[1] + " NAME '.' LINK TO INODE " + 
                dirent.split(',')[3] + " SHOULD BE " + dirent.split(',')[1])
        if dirent.split(',')[6] == "'..'":
            if int(dirent.split(',')[3]) != parentDir[int(dirent.split(',')[1])]:
                inconsistency = True
                print("DIRECTORY INODE " + dirent.split(',')[1] + " NAME '..' LINK TO INODE " + 
                dirent.split(',')[3] + " SHOULD BE " + str(parentDir[int(dirent.split(',')[1])]))
           
            
    for item in inodes:
        if int(item.split(',')[6]) != inodeReferences[int(item.split(',')[1])]:
            inconsistency = True
            print("INODE " + item.split(',')[1] + " HAS " + str(inodeReferences[int(item.split(',')[1])])
            + " LINKS BUT LINKCOUNT IS " + item.split(',')[6])
    
    

if __name__ == '__main__':
    findErrors()
