#include <ros/ros.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/JointState.h>

//using namespace message_filters;

ros::Publisher pub;

void callback(const sensor_msgs::JointStateConstPtr &left_joint_states, const sensor_msgs::JointStateConstPtr &right_joint_states)
{
	unsigned int i;
	sensor_msgs::JointState joints_agg;
	joints_agg.header.stamp = ros::Time::now();

	bool use_position = !(left_joint_states->position.empty() || right_joint_states->position.empty());
	bool use_velocity = !(left_joint_states->velocity.empty() || right_joint_states->velocity.empty());
	bool use_effort = !(left_joint_states->effort.empty() || right_joint_states->effort.empty());

	for(i = 0; i < left_joint_states->name.size(); i++)
	{
		joints_agg.name.push_back(left_joint_states->name[i]);

		if(use_position)
			joints_agg.position.push_back(left_joint_states->position[i]);

		if(use_velocity)
			joints_agg.velocity.push_back(left_joint_states->velocity[i]);

		if(use_effort)
			joints_agg.velocity.push_back(left_joint_states->effort[i]);
	}

	for(i = 0; i < right_joint_states->name.size(); i++)
	{
		joints_agg.name.push_back(right_joint_states->name[i]);

		if(use_position)
			joints_agg.position.push_back(right_joint_states->position[i]);

		if(use_velocity)
			joints_agg.velocity.push_back(right_joint_states->velocity[i]);

		if(use_effort)
			joints_agg.velocity.push_back(right_joint_states->effort[i]);
	}

	pub.publish(joints_agg);
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "joint_state_aggregator");

	ros::NodeHandle nh;
	message_filters::Subscriber<sensor_msgs::JointState> left_joint_sub(nh, "left_joint_states", 1);
	message_filters::Subscriber<sensor_msgs::JointState> right_joint_sub(nh, "right_joint_states", 1);

	message_filters::Synchronizer<message_filters::sync_policies::ApproximateTime<sensor_msgs::JointState, sensor_msgs::JointState> >
		sync(message_filters::sync_policies::ApproximateTime<sensor_msgs::JointState, sensor_msgs::JointState>(2), left_joint_sub, right_joint_sub);

	sync.registerCallback(boost::bind(&callback, _1, _2));

	pub = nh.advertise<sensor_msgs::JointState>("joint_states_agg", 1);

	ros::spin();

	return 0;
}
