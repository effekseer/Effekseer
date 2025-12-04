#include "efkMat.CommandManager.h"

namespace EffekseerMaterial
{
DelegateCommand::DelegateCommand(const std::function<void()>& execute, const std::function<void()>& unexecute)
	: execute_(execute)
	, unexecute_(unexecute)
{
}

DelegateCommand::~DelegateCommand()
{
}

void DelegateCommand::Execute()
{
	execute_();
}

void DelegateCommand::Unexecute()
{
	unexecute_();
}

CommandCollection::CommandCollection()
{
}
CommandCollection::~CommandCollection()
{
}

void CommandCollection::AddCommand(std::shared_ptr<ICommand> command)
{
	if (commands_.size() > 0)
	{
		if (command->Merge(commands_.back().get()))
		{
			commands_.back() = command;
		}
		else
		{
			commands_.push_back(command);
		}
	}
	else
	{
		commands_.push_back(command);
	}
}

void CommandCollection::Execute()
{
	for (auto c : commands_)
	{
		c->Execute();
	}
}

void CommandCollection::Unexecute()
{
	for (int32_t i = static_cast<int32_t>(commands_.size()) - 1; i >= 0; i--)
	{
		commands_[i]->Unexecute();
	}
}

void CommandManager::StartCollection()
{
	if (collectionCount_ == 0)
	{
		collection_ = std::make_shared<CommandCollection>();
	}

	collectionCount_++;
}

void CommandManager::EndCollection()
{
	collectionCount_--;

	if (collectionCount_ == 0)
	{
		Execute(collection_);
		collection_ = nullptr;
	}
}

void CommandManager::Execute(std::shared_ptr<ICommand> command)
{
	if (collectionCount_ > 0)
	{
		collection_->AddCommand(command);
		command->Execute();
	}
	else
	{
		commands_.resize(commandInd_);

		if (commands_.size() > 0)
		{
			if (isMergeEnabled_ && command->Merge(commands_.back().get()))
			{
				commands_.back() = command;
			}
			else
			{
				commands_.push_back(command);
				commandInd_++;
			}
		}
		else
		{
			commands_.push_back(command);
			commandInd_++;
		}

		command->Execute();
		isMergeEnabled_ = true;
	}

	historyId_++;
}

void CommandManager::Undo()
{
	if (collectionCount_ > 0)
		return;
	if (commandInd_ == 0)
		return;
	commands_[commandInd_ - 1]->Unexecute();
	commandInd_--;
	historyId_++;
}

void CommandManager::Redo()
{
	if (collectionCount_ > 0)
		return;
	if (commandInd_ == commands_.size())
		return;
	commands_[commandInd_]->Execute();
	commandInd_++;
	historyId_++;
}

void CommandManager::Reset()
{
	commands_.clear();
	commandInd_ = 0;
	collectionCount_ = 0;
	historyId_ = 0;
}

void CommandManager::MakeMergeDisabled()
{
	isMergeEnabled_ = false;
}

uint64_t CommandManager::GetHistoryID()
{
	return historyId_;
}

} // namespace EffekseerMaterial
