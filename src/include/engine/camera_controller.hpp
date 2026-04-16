class Camera;

class CameraController {
  public:
    CameraController(Camera& camera);
    void handleInput(float deltaTime);
private:
  void onMouseMove(float x, float y);
  void onScrollMove(float xOffset, float yOffset);

   Camera& camera_;

   bool firstMouse_ = true;
   float lastX_, lastY_;
};