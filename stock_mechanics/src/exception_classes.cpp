#include "exception_classes.hpp"

using namespace ProjectStockMarket;

UserNotFound::UserNotFound(const std::string p)  :  m_message(p){}

ProjectStockMarket::ProductNotFound::ProductNotFound(const std::string p)  :  m_message(p)
{
}

ProjectStockMarket::NotEnoughMoney::NotEnoughMoney(const std::string p)  :  m_message(p)
{
}

ProjectStockMarket::OutOfStock::OutOfStock(const std::string p)  :  m_message(p)
{
}

ProjectStockMarket::IncorrectPassword::IncorrectPassword(const std::string p)  :  m_message(p)
{
}

ProjectStockMarket::InvalidToken::InvalidToken(const std::string p)  :  m_message(p)
{
}

ProjectStockMarket::AccountAlreadyExists::AccountAlreadyExists(const std::string p) : m_message(p)
{
}


