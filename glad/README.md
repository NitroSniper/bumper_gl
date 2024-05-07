# Getting Glad for the Project
### Preface
This project is using `glad2` instead of `glad1`.
The only difference I see between them is the loader code and the lib name
```c++
// Glad2
#include <glad/gl.h>
if (!gladLoadGL(glfwGetProcAddress)) {
    throw std::runtime_error("Error initializing glad");
}

// Glad1
#include <glad/glad.h>
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Error initializing glad");
}
```
I chose `glad2` but you can use either.
## Getting Glad for the Project
Get the glad source code (via the link) and place it in the `glad/` folder. the file structure should look the same as the one below.
* `glad1` - https://glad.dav1d.de/

  ```md
  glad
  ├── include
  │   ├── glad/glad.h
  │   └── KHR/khrplatform.h
  └── src/glad.c
  ```
* `glad2` - https://gen.glad.sh/
  ```md
  glad
  ├── include
  │   ├── glad/gl.h
  │   └── KHR/khrplatform.h
  └── src/gl.c
  ```



