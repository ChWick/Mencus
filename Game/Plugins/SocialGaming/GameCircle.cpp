#include "GameCircle.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include <android/log.h>

using namespace AmazonGames;

using namespace GameCircle;
  
GetAchCbImpl::GetAchCbImpl() {
}
  
GetAchCbImpl::~GetAchCbImpl() {
}
 
void GetAchCbImpl::onGetAchievementCb(ErrorCode errorCode,
				      const AchievementData* responseStruct,
				      int developerTag) {
  // Your callback action code
  //
  // Example struct access:
  __android_log_write(ANDROID_LOG_DEBUG, "GetAchCbImpl", responseStruct->id);
}


#endif // MENCUS_USE_AMAZON_GAME_CIRCLE
