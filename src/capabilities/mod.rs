use crate::capabilities::kernel::Kernel;
use crate::capabilities::user::User;

mod kernel;
mod user;

pub struct Capabilities {
    description: String,
    kernel: Kernel,
    user: User,
}