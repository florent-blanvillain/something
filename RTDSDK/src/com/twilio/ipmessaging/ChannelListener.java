package com.twilio.ipmessaging;

import java.util.Map;

/**
 * Interface to report channel operations such as member join/leave and add/delete/update messages. 
 * 
 * @author kbagchi
 *
 */

public interface ChannelListener {
	
	/**
	 * Called when a Message is added to the channel the current user is subscribed to.
	 * 
	 * @param message
	 */
	void onMessageAdd(Message message);
	
	/**
	 * Called when a Message is changed for the channel the current user is subscribed to.
	 * 
	 * @param message The message that changed.
	 */
	void onMessageChang(Message message);   
	
	/**
	 * Called when a Message is deleted from the channel the current user is subscribed to.
	 * 
	 * @param message The message that got deleted.
	 */
	void onMessageDelete(Message message);
	
	/**
	 * Called when a Member is added to the channel the current user is subscribed to.
	 * 
	 * @param member The member that joined.
	 */
	void onMemberJoin(Member member);
	
	/**
	 * Called when a Member is changed in the channel the current user is subscribed to.
	 * 
	 * @param member The member that changed.
	 */
	void onMemberChange(Member member);
	
	/**
	 * Called when a Member is deleted from the channel the current user is subscribed to.
	 * 
	 * @param member The member that was deleted.
	 */
	void onMemberDelete(Member member);
	
	/**
	 * Called when channel attributes have changed.
	 * 
	 * @param updatedAttributes updated channel attribute. 
	 */
	void onAttributesChange(Map<String, String> updatedAttributes);
}