#include <string>
#include <linux/videodev2.h>


struct resolution
{
    unsigned int width = 640;
    unsigned int height = 480;
};

typedef struct buffer
{
    void *start;
    unsigned int length;
} Buffer;

class YuvCapturer
{
public:
    YuvCapturer(resolution s);
    void init();
    int camera_capture_start();
   // void print_camera_message();
    void set_resolution_(resolution s);
    resolution get_resolution() { return resolution_; };
    Buffer get_one_frame();
    void clear_one_frame();


private:
    resolution resolution_;
    std::string deviceName_ = "/dev/video0";
    int camera_fd_ = -1;
    int buffNum_ = 4;    // 申请的帧缓冲个数
    Buffer* buffers_ = nullptr;
    struct v4l2_buffer buf;
    int fps_ = 30;

private:
    int camera_open();
    int camera_ioctl(int request, void *arg);
    int camera_query_cap();
    int camera_set_video_fmt();
    int camera_request_buffer();
    int camera_buffer_release(int i);
    int camera_set_fps();
};

int YUV422To420(unsigned char yuv422[], unsigned char yuv420[], int width, int height);
