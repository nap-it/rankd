use warp::Filter;
use slog::{Logger, info};
use json::object;
use crate::node::System;

pub async fn start(log: Logger) {
    info!(log, "Successfully loaded system information.");

    let testing_tool = warp::path::end().map(|| {
        format!("Welcome to this service")
    });

    let kernel_info = warp::path("os").map(|| {
        format!("{}", object!{
            code : 200,
            data : System::global(),
        })
    });

    let userspace_info = warp::path("user").map(|| {
        format!("{:#?}", crate::capabilities::Capabilities::user())
    });

    let network_info = warp::path("network").map(|| {
        format!("{:#?}", crate::capabilities::Capabilities::user().network())
    });

    let network_interface_info = warp::path!("networks" / String).map(|interface| {
         format!("{:#?}", crate::capabilities::Capabilities::user().network_interface(interface).unwrap())
    });

    let routes = warp::get().and(
        kernel_info
            .or(userspace_info)
            .or(network_info)
            .or(network_interface_info)
    );

    warp::serve(routes).run(([127, 0, 0, 1], 7265)).await;
}