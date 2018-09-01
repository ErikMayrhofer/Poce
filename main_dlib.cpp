#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/geometry/vector.h>
#include "monitoring.h"

using namespace dlib;
using namespace std;

int main()
{
    try
    {
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

        //image_window win;

        // Load face detection and pose estimation models.
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        cv::Mat temp;
        START_MONITOR(now)
        while(true)
        {
            MONITOR("Start", now)
            cap >> temp;
            MONITOR("CameraUpdate", now)


            cv_image<bgr_pixel> cimg(temp);

            auto now = std::chrono::system_clock::now();
            std::vector<rectangle> dets = detector(cimg);
            auto diff = std::chrono::system_clock::now() - now;

            MONITOR("Detect", now);

            std::vector<dlib::point> noses;
            for (const auto &face : dets){
                full_object_detection detection = pose_model(cimg, face);
                dlib::point noseTip = detection.part(33);
                cv::Point pos = cv::Point2d(noseTip.x(), noseTip.y());
                cv::circle(temp, pos, 10, cv::Scalar(255, 255, 255));
            }
            MONITOR("Draw", now);

            cv::imshow("Input", temp);
            if(cv::waitKey(1) == 27){
                cout << "End!" << endl;
                break;
            }

        }
    }
    catch(serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}

