# hair-filter-app
An Android application that applies 3D hair filters on a person's head.

We can add hair filters to people by rendering 3D hair models in the image and placing it on the person's head.

<h3>Features</h3>

- Add the hair filter to both front and back cameras
- Select multiple different 3D hair models
- Select different color filters for the hair
- Take a picture and save the image

<img src="https://github.com/kim2552/hair-filter-app/blob/main/assets/readme_gif.gif" width="200" />

<h3>Description</h3>

Developed in Android Studio. Written in Java and C++. Uses OpenGL, OpenCV and Dlib libraries.

The image and 3D hair model is rendered using OpenGL. The 3D hair models used are .obj files and are processed using [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader).

The image is processed through a face cascade model and shape predictor model to detect a person's face and get face landmark points. We also detect the face pose in the image to determine the angle position of the hair model.

The face landmark points are mapped to a pre-configured point in the hair model. This allows the hair model to be dynamically placed on the persons head.

An outline of the peron's face and neck is resized, cropped and displayed a distance away from the original image. The 3D hair model is placed on the outline. This allows the hair to cover parts of the image that does not belong to the face.

Most of the image processing and image rendering code was first developed in [hair-filter-sandbox](https://github.com/kim2552/hair-filter-sandbox) and then later developed as an Android application.
