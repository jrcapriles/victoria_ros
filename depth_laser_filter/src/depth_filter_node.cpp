#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "message_filters/subscriber.h"
#include "tf/message_filter.h"
#include "tf/transform_listener.h"
#include "filters/filter_chain.h"

class GenericLaserScanFilterNode
{
protected:
  // Our NodeHandle
  ros::NodeHandle nh_;

  // Components for tf::MessageFilter
  tf::TransformListener tf_;
  message_filters::Subscriber<sensor_msgs::LaserScan> scan_sub_;

  // Components for publishing
  sensor_msgs::LaserScan msg_;
  ros::Publisher output_pub_;

  ros::Timer deprecation_timer_;

  double min_th_;


public:
  // Constructor
  GenericLaserScanFilterNode() :
    scan_sub_(nh_, "scan_depth", 50), 
    min_th_(0.6)
  {
    scan_sub_.registerCallback(boost::bind(&GenericLaserScanFilterNode::callback, this, _1));
    
    // Advertise output
    output_pub_ = nh_.advertise<sensor_msgs::LaserScan>("scan_depth_cleaner", 1000);
  }
  
  // Callback
  void callback(const sensor_msgs::LaserScan::ConstPtr& msg_in)
  {
    // Copy the incoming laser scan
    msg_ = *msg_in;

    // Iterate over the range readings and substitute nan or small readings with max range
    for (unsigned int i = 0; i < msg_.ranges.size(); i++)
    {
	if( (msg_.ranges[i] <= min_th_) || (msg_.ranges[i] >  msg_.range_max) || (isnan(msg_.ranges[i])))
	{
	    msg_.ranges[i] = msg_.range_max;
	}
    }
	                         
    // Publish the output
    output_pub_.publish(msg_);
  }
};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "depth_scan_filter_node");
  
  GenericLaserScanFilterNode t;
  ros::spin();
  
  return 0;
}



  



