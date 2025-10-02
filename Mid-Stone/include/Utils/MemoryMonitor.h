#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include <iostream>
#include <memory>

#include <vector>
#include <string>

inline std::vector<void*> memoryStack;
inline std::vector<std::pair<std::string, void*>> printStorage;
inline unsigned int memoryStackCounter = 0;
inline int amountOfMemoryAllocated = 0;

inline std::vector<void*> memoryStackARR;
inline unsigned int memoryStackCounterARR = 0;
inline std::vector<std::pair<std::string, void*>> printStorageARR;
inline int amountOfMemoryAllocatedARR = 0;

inline void* operator new(std::size_t numBytes, const char* FILE, const int LINE) {
  
    void* ptr = std::malloc(numBytes);
    memoryStack.push_back(ptr);
    amountOfMemoryAllocated += numBytes;
    std::string msg = std::string(FILE) + " : " + std::to_string(LINE) +
        " : Allocated " + std::to_string(numBytes) + " bytes of memory"  + "\n";
    printStorage.push_back(std::make_pair(msg, ptr));
    return ptr;
}

inline void operator delete(void* memoryLocation, std::size_t numBytes) {
    auto it = std::find(memoryStack.begin(), memoryStack.end(), memoryLocation);
    if (it != memoryStack.end()) {
        auto msgIt = std::find_if(printStorage.begin(), printStorage.end(),
            [memoryLocation](const std::pair<std::string, void*>& pair) {
                return pair.second == memoryLocation;
            });

        if (msgIt != printStorage.end()) {
            std::cout << msgIt->first;
            printStorage.erase(msgIt);
            
        }
        
        std::cout << "Previous amount of memory allocated before delete : " << amountOfMemoryAllocated;
        std::cout << "\n: deleted memory location: " << memoryLocation;
        if (amountOfMemoryAllocated >= numBytes) {
            amountOfMemoryAllocated -= numBytes;
        }
        else {
            // Log an error or warning here — something's wrong
            std::cerr << "Warning: Attempt to delete more memory than allocated!" << std::endl;
            amountOfMemoryAllocated = 0; 
        }

        std::cout << " : amount of memory deleted: " << numBytes;
        std::cout << " : Current amount of memory allocated : " << amountOfMemoryAllocated << std::endl << std::endl;

        memoryStack.erase(it);
        
    }
    std::free(memoryLocation);
}

/// This is a trick to get access to numBytes 
/// w/o wild typecasts. 
//struct ArraySize {
//    std::size_t numBytes;
//};
//
// ///I did a little hack to hide the total number of bytes
///// allocated in the array itself. 
//inline void* operator new[](std::size_t numBytes, const char* FILE, const int LINE) {
//  
//
//    ArraySize* array = reinterpret_cast<ArraySize*>(std::malloc(numBytes + sizeof(ArraySize)));
//    if (array) array->numBytes = numBytes;
//    void* ptr = array + 1;
//    memoryStackARR.push_back(ptr);
//    amountOfMemoryAllocatedARR += numBytes;
//
//    std::string msg = std::string(FILE) + " : " + std::to_string(LINE) +
//        " : Allocated ARRAY " + std::to_string(numBytes) + " bytes of memory ARRAY" + "\n";
//    printStorageARR.push_back(std::make_pair(msg, ptr));
//    return array + 1;
//
//
//}
//
///// This overload doesn't work as advertised in VS22
//inline void operator delete[](void* memoryLocation) {
//    ArraySize* array = reinterpret_cast<ArraySize*>(memoryLocation) - 1;
//
//
//    auto it = std::find(memoryStackARR.begin(), memoryStackARR.end(), memoryLocation);
//    if (it != memoryStackARR.end()) {
//
//        auto it = std::find(memoryStackARR.begin(), memoryStackARR.end(), memoryLocation);
//        if (it != memoryStackARR.end()) {
//            auto msgIt = std::find_if(printStorageARR.begin(), printStorageARR.end(),
//                [memoryLocation](const std::pair<std::string, void*>& pair) {
//                    return pair.second == memoryLocation;
//                });
//
//            if (msgIt != printStorageARR.end()) {
//                std::cout << msgIt->first;
//                printStorageARR.erase(msgIt);
//
//            }
//           //    memoryStackARR.erase(it);
//
//            std::cout << "Previous amount of memory allocated before delete ARRAY : " << amountOfMemoryAllocatedARR;
//            std::cout << "\n: deleted memory location ARRAY: " << memoryLocation;
//            if (amountOfMemoryAllocatedARR >= array->numBytes) {
//                amountOfMemoryAllocatedARR -= array->numBytes;
//            }
//            else {
//                // Log an error or warning here — something's wrong
//                std::cerr << "Warning: Attempt to delete more memory than allocated!" << std::endl;
//                amountOfMemoryAllocatedARR = 0; // Or handle as appropriate
//            }
//            std::cout << " : amount of memory deleted ARRAY : " << array->numBytes;
//            std::cout << " : Current amount of memory allocated ARRAY : " << amountOfMemoryAllocatedARR << std::endl << std::endl;
//
//
//        }
//
//    }
//    free(array);
//}

#define new new(__FILE__, __LINE__)


#endif

