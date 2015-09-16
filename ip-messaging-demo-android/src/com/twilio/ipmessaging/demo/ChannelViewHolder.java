package com.twilio.ipmessaging.demo;

import com.twilio.example.R;
import com.twilio.ipmessaging.Channel;

import android.view.View;
import android.widget.TextView;
import uk.co.ribot.easyadapter.ItemViewHolder;
import uk.co.ribot.easyadapter.PositionInfo;
import uk.co.ribot.easyadapter.annotations.LayoutId;
import uk.co.ribot.easyadapter.annotations.ViewId;

@LayoutId(R.layout.channel_item_layout)
public class ChannelViewHolder extends ItemViewHolder<Channel> {

	@ViewId(R.id.channel_friendly_name)
	TextView friendlyName;

	@ViewId(R.id.channel_sid)
	TextView channelSid;

	View view;

	public ChannelViewHolder(View view) {
		super(view);
		this.view = view;
	}

	@Override
	public void onSetListeners() {
		view.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				OnChannelClickListener listener = getListener(OnChannelClickListener.class);
				if (listener != null) {
					listener.onChannelClicked(getItem());
				}
			}
		});
	}

	@Override
	public void onSetValues(Channel channel, PositionInfo arg1) {
		friendlyName.setText(channel.getFriendlyName());
		channelSid.setText(channel.getSid());
	}

	public interface OnChannelClickListener {
		void onChannelClicked(Channel channel);
	}
}
