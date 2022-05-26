#include "monocular-slam-node.hpp"

#include <opencv2/core/core.hpp>

using ImageMsg = sensor_msgs::msg::Image;

//mspm
//using PointCloud = sensor_msgs::msg::PointCloud


using namespace std;

using std::placeholders::_1;

MonocularSlamNode::MonocularSlamNode(ORB_SLAM3::System* pSLAM, const string &strVocFile, const string &strSettingsFile)
:   rclcpp::Node("orbslam"), m_SLAM(pSLAM)
{

    m_image_subscriber = this->create_subscription<ImageMsg>(
        "camera", 
        rclcpp::SensorDataQoS(),
        std::bind(&MonocularSlamNode::GrabImage, this, std::placeholders::_1)
    );

}


MonocularSlamNode::~MonocularSlamNode()
{
    // Stop all threads
    m_SLAM->Shutdown();

    // Save camera trajectory
    m_SLAM->SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

}


void MonocularSlamNode::GrabImage(const ImageMsg::SharedPtr msg)
{
    RCLCPP_INFO(this->get_logger(), "GrabImage");

    // Copy the ros image message to cv::Mat.
    try
    {
        m_cvImPtr = cv_bridge::toCvCopy(msg);
    }
    catch (cv_bridge::Exception& e)
    {
        RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
        return;
    }

    
    // cv::Mat Tcw = m_SLAM->TrackMonocular(m_cvImPtr->image, msg->header.stamp.sec);
    cv::Mat Tcw;
    Sophus::SE3f Tcw_SE3f = m_SLAM->TrackMonocular(m_cvImPtr->image, msg->header.stamp.sec);  // <=====================
    Eigen::Matrix4f Tcw_Matrix = Tcw_SE3f.matrix();
    cv::eigen2cv(Tcw_Matrix, Tcw);

}
